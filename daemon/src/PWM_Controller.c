
// *****************
#define _XOPEN_SOURCE

// *****************
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>

#include "parson.h"
#include "PWM_Controller.h"
#include "Serial.h"
#include "Command_List.h"

// *****************
const char PWM_Con_Settings_file[] = "/etc/PWM_Controller.conf";

JSON_Value *JSON_Settings;
JSON_Object *J_Object;

int Serial_fd;

#define SERIAL_BUF_SIZE 4096
char SerialRead_Buf[SERIAL_BUF_SIZE];

//CmdFuncList_t Cmd_List;

#define SYSTEM_DELAY	10000
const struct timeval system_time = {0,SYSTEM_DELAY};

#define TIMER_TICK		100000 // ms

// *****************
void Setup_Timer(int start);

// *****************
// Options
//#define __DAEMONISE__		1
// *****************
void System_Shutdown(void)
{
    CL_CLearSharedMemory();
    syslog(LOG_EMERG, "System shutting down");
    closelog();
#ifndef __DAEMONISE__
    printf("Shutdown\n");
#endif
}

// *****************
void Check_Serial(int rv)
{
    if ( rv < 0 )
    {
        Setup_Timer(0);
        CL_SetDisconnected();
        Serial_fd = Serial_ClosePort(Serial_fd);
        syslog(LOG_EMERG, "Serial coms lost, %d", errno);
#ifndef __DAEMONISE__
        printf("Serial Lost\n");
#endif
    }
}

// *****************
void Read_Settings(void)
{
    int rv;

    JSON_Settings = json_parse_file(PWM_Con_Settings_file);
    rv = json_value_get_type(JSON_Settings);

    if ( rv != JSONObject )
    {
        syslog(LOG_EMERG, "JSON data incorrect, %d\n", rv );
        exit( -1);
    }

    J_Object = json_value_get_object(JSON_Settings);
    if ( J_Object == NULL )
    {
        syslog(LOG_EMERG, "JSON: Failed to get object" );
        exit( -1);
    }
}

// *****************
void Connect_To_Port(void)
{
    const char *name = json_object_get_string(J_Object, "Serial_Port");

    if ( name == NULL )
    {
        syslog(LOG_NOTICE, "Com Port name not set");
        exit(-1);
    }

    Serial_fd = Serial_Openport((char *)name, 115200, 0,0 );
    if ( Serial_fd < 0 )
        return;
    syslog(LOG_NOTICE, "Com Port %s connected", name);
#ifndef __DAEMONISE__
    printf("Port Running\n");
#endif
    CL_SetConnected();
    Setup_Timer(1);

    // send start up commands.
    // read time
    Send_GetTime(Serial_fd);
    // send temp limit,
    // send temp update rate
    Send_TempData(Serial_fd, J_Object);

    // send current limit,
    // send current update rate
    Send_CurrentData(Serial_fd, J_Object);

    Send_VoltData(Serial_fd, J_Object);

    // force unit to restart
    Send_Restart(Serial_fd);
#ifndef __DAEMONISE__
    printf("Restart sent\n");
#endif
}

// *****************
void RunTimer(int sig)
{   // receive signal, set up next tick.
    static int write_time;
    struct timeval tv;
    int rv = 1;

    if ( Serial_fd < 0 )
        return ;	// serial port not configured so don't bother

    gettimeofday(&tv, NULL);
    if (( tv.tv_sec - write_time ) > 3600)
    {   // here once an hour
        write_time = tv.tv_sec;
        syslog(LOG_DEBUG, "Sending Time data" );
        rv = Send_SystemTime(Serial_fd, write_time);
        Check_Serial(rv);
    }

    rv = Send_PWMChanelData(Serial_fd );
    Check_Serial(rv);
}

// *****************
void Setup_Timer(int start)
{
    struct itimerval timer;
    struct sigaction sig;

    // Install timer_handler as the signal handler for SIGVTALRM.
    memset (&sig, 0, sizeof (struct sigaction));
    sig.sa_handler = &RunTimer;
    sigaction (SIGALRM , &sig, NULL);

    if ( start > 0 )
    {
        // Configure the timer to expire after 25 msec...
        timer.it_value.tv_sec = 0;
        timer.it_value.tv_usec = TIMER_TICK;
        // ... and every 25 msec after that.
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_usec = TIMER_TICK;
    }
    else
    {   // stop the timer
        timer.it_value.tv_sec = 0;
        timer.it_value.tv_usec = 0;
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_usec = 0;
    }

    // Start a virtual timer.
    setitimer (ITIMER_REAL, &timer, NULL);
}

// *****************
void Run_CtrlC(int sig)
{
    exit(0);
}

// *****************
void Setup_SignalHandler(void)
{
    struct sigaction sig;

    // Install timer_handler as the signal handler for SIGVTALRM.
    memset (&sig, 0, sizeof (struct sigaction));
    sig.sa_handler = &Run_CtrlC;
    sigaction (SIGINT , &sig, NULL);
    sigaction (SIGTERM , &sig, NULL);
    sigaction (SIGABRT , &sig, NULL);
}

// *****************
int main(int argc, char *argv[])
{
    int loop = 1;
    int rv;
    int length;
    fd_set readfds;
    struct timeval select_time;

    openlog("PWM_Controller", LOG_PID , LOG_USER );
    syslog(LOG_NOTICE, "PWM_Controller Startup");


    // register shutdown function.
    atexit(System_Shutdown);

    CL_Create_Shared_Memory();
    Read_Settings();
//  Cmd_List = CmdParse_CreateFuncList();
//	Build_CmdList(Cmd_List);
#ifdef __DAEMONISE__
    rv = daemon( 0, 0 );
    if ( rv < 0 )
    {
        syslog(LOG_EMERG, "Daemonise failed" );
        exit(-1);
    }
#else
    printf("System starting - Debug mode\n");
#endif

    Setup_SignalHandler();

    while ( loop > 0 )
    {
        Connect_To_Port();
        while ( Serial_fd >= 0 )
        {
            FD_ZERO(&readfds);
            FD_SET(Serial_fd, &readfds);
            select_time = system_time;

            rv = select(Serial_fd+1, &readfds, NULL, NULL, &select_time);	// wait indefinately
            if ( rv < 0 )
                continue;

            if ( FD_ISSET(Serial_fd, &readfds) )
            {
                // run receiver
                length = strlen(SerialRead_Buf);
                rv = read(Serial_fd, SerialRead_Buf + length, SERIAL_BUF_SIZE - length);
                Check_Serial(rv);

                if ( rv > 0 )
                {
                    SerialRead_Buf[length + rv] = 0;
#ifndef __DAEMONISE__
                    puts(SerialRead_Buf+length);
#endif
                    while ( rv >= 0 )
                    {
                        rv = CmdParse_ProcessString(Cmd_Table, SerialRead_Buf, Serial_fd);
                    }
                }
            }
        }
#ifndef __DAEMONISE__
        printf("Sleep\n");
#endif
        sleep(60);
    }
    return 0;
}

// *****************
// *****************

