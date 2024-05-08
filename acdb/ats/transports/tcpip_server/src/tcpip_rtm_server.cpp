/**
*=============================================================================
*  \file tcpip_rtm_server.cpp
*  \brief
*                      T C P I P  R T M  S E R V E R  
*                         S O U R C E  F I L E
*
*    This file contains the RTM server implementation to host connections
*    to QACT.
*    This file first sets up a listening socket on port 5560. Upon accepting
*    new connections, the server sends RTM packets to the connected client.
*
*  \cond
*      Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*      SPDX-License-Identifier: BSD-3-Clause
*  \endcond
*=============================================================================
*/
#ifdef ATS_TRANSPORT_TCPIP

#include "tcpip_rtm_server.h"
#if defined(FEATURE_ATS_PUSH)

#define TCPIP_THREAD_PRIORITY_HIGH 0
#define TCPIP_THREAD_PRIORITY_LOW 1
#define TCPIP_THD_STACK_SIZE 0xF4240 //1mb stack size

#define TCPIP_RTM_SERVER_PORT 5560
#define MAX_TCPIP_CLIENTS_ALLOWED 1

/**< ATS TCPIP RTM Server(TRS) Log Tag */
#define TCPIP_RTM_SRV_LOG_TAG  "ATS-TRS"
#define TCPIP_RTM_ERR(...) AR_LOG_ERR(TCPIP_RTM_SRV_LOG_TAG, __VA_ARGS__)
#define TCPIP_RTM_DBG(...) AR_LOG_DEBUG(TCPIP_RTM_SRV_LOG_TAG, __VA_ARGS__)
#define TCPIP_RTM_INFO(...) AR_LOG_INFO(TCPIP_RTM_SRV_LOG_TAG, __VA_ARGS__)

typedef void* (TcpipRtmServer::* rtm_server_thread_callback)(void* args);
static rtm_server_thread_callback thd_cb_connect_routine = &TcpipRtmServer::connect_routine;

/*
============================================================================
                        RTM Server Implementation
============================================================================
*/

TcpipRtmServer::TcpipRtmServer(std::string thd_name)
{
}

int32_t TcpipRtmServer::start()
{
    int32_t status = AR_EOK;
    std::string thd_name = "RTM_SVR";

    status = ar_osal_mutex_create(&connection_lock);
    if (AR_FAILED(status))
    {
        TCPIP_RTM_ERR("Error[%d]: Failed to create mutex for %s thread", status, thd_name.c_str());
        return status;
    }

    ar_osal_thread_attr_t thd_attr =
    {
        (char_t*)thd_name.c_str(),
        TCPIP_THD_STACK_SIZE,
        TCPIP_THREAD_PRIORITY_HIGH,
    };

    ar_osal_thread_start_routine r = \
        (ar_osal_thread_start_routine)(connect);

    status = ar_osal_thread_create(&thread, &thd_attr, r, this);
    if (AR_FAILED(status))
    {
        TCPIP_RTM_ERR("Error[%d]: Failed to create thread %s", status, thd_name.c_str());
        return status;
    }

    return status;
}

int32_t TcpipRtmServer::stop()
{
    if (!is_connected)
        return AR_EOK;

    TCPIP_RTM_INFO("Closing RTM Server ...");
    int32_t status = ar_osal_thread_join_destroy(thread);
    if (AR_FAILED(status))
    {
        TCPIP_RTM_ERR("Error[%d]: Failed to destroy connection thread", status);
    }

    should_close_server = true;
    ar_socket_close(accept_socket);
    ar_osal_mutex_destroy(connection_lock);

    return AR_EOK;
}

int32_t TcpipRtmServer::set_connected_lock(uint8_t value)
{
    int32_t status = AR_EOK;

    status = ar_osal_mutex_lock(connection_lock);
    if (AR_FAILED(status))
    {
        return status;
    }

    is_connected = value;

    status = ar_osal_mutex_lock(connection_lock);
    if (AR_FAILED(status))
    {
        return status;
    }

    return status;
}

void TcpipRtmServer::connect(void* arg)
{
    TcpipRtmServer* thread = (TcpipRtmServer*)arg;
    (thread->*thd_cb_connect_routine)(NULL);
}

void* TcpipRtmServer::connect_routine(void* args)
{
    __UNREFERENCED_PARAM(args);

    TCPIP_RTM_INFO("Starting RTM server thread.");
    int32_t status = AR_EOK;
    char_t* str_addr = (char_t*)address.c_str();
    ar_socket_t listen_socket;

    status = tcpip_socket_util_create_socket(AR_SOCKET_ADDRESS_FAMILY, str_addr, port, &listen_socket);
    if (AR_FAILED(status))
    {
        TCPIP_RTM_ERR("Error[%d]: Failed to create and bind to socket", status);
        return NULL;
    }

    //TCPIP_RTM_INFO("%s: Listening for connections...", thd_name.c_str());

    /* Clients will recieve a 'connection refused' error if the the
     * listening queue is full. The size of the queue is
     * MAX_TCPIP_CLIENTS_ALLOWED */
    status = ar_socket_listen(listen_socket, MAX_TCPIP_CLIENTS_ALLOWED);
    if (AR_SUCCEEDED(status))
    {
        TCPIP_RTM_INFO("Listening successful");
    }
    else
        return NULL;

    status = set_connected_lock(false);
    if (AR_FAILED(status))
    {
        TCPIP_RTM_ERR("Error[%d]: Failed to set 'connected' lock", status);
        return 0;
    }

    while (true)
    {
        TCPIP_RTM_INFO("Waiting to accept RTM clients...");

        status = tcpip_socket_util_accept_connections(AR_SOCKET_ADDRESS_FAMILY, listen_socket, &accept_socket);
        if (AR_FAILED(status))
        {
            ar_socket_close(accept_socket);
            if (AR_SOCKET_LAST_ERROR == EINTR) continue;

            continue;
        }

        ar_osal_mutex_lock(connection_lock);
        if (is_connected)
        {
            TCPIP_RTM_ERR("New connection request. Refusing...");
            ar_socket_close(accept_socket);
            ar_osal_mutex_unlock(connection_lock);
            continue;
        }
        ar_osal_mutex_unlock(connection_lock);

        TCPIP_RTM_INFO("Client connected to RTM Server...");
        break;
    }

    ar_socket_close(listen_socket);

    return 0;
}

int32_t TcpipRtmServer::send_rtm_data(const char_t* buffer, uint32_t buffer_size)
{
    int32_t status = AR_EOK;
    int32_t bytes_sent = 0;

    status = ar_socket_send(accept_socket, buffer, buffer_size, 0, &bytes_sent);
    if (AR_SUCCEEDED(status) && bytes_sent >= 0)
    {
        TCPIP_RTM_DBG("Sent %d bytes:", bytes_sent);
        return status;
    }

    return AR_EOK;
}

#endif /*#if defined(ATS_RTM_PUSH)*/

#endif /*ATS_TRANSPORT_TCPIP*/