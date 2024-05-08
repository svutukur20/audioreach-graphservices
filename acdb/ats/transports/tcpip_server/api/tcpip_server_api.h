#ifndef _TCPIP_CMD_SERVER_API_H_
#define _TCPIP_CMD_SERVER_API_H_
/**
*=============================================================================
* \file tcpip_cmd_server.h
* \brief
*              A T S  T C P I P  C M D  S E R V E R  
*                      H E A D E R  F I L E
*
*   This file contains the TCP/IP server API definitions used to host
*   connections to QACT.
*
* \copyright
*     Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*     SPDX-License-Identifier: BSD-3-Clause
*
*=============================================================================
*/

/*------------------------------------------
* Includes
*------------------------------------------*/
#include "ar_osal_mutex.h"
#include "ar_osal_error.h"
#include "ar_osal_thread.h"
#include "ats_common.h"
#include "acdb_utility.h"

#ifdef __cplusplus
extern "C"{
#endif /*__cplusplus*/

#ifdef ATS_TRANSPORT_TCPIP
    typedef struct tcpip_cmd_server_context {
        void(*ats_cb)(uint8_t* req, uint32_t req_len, uint8_t** resp, uint32_t* resp_len);
    } tcpip_cmd_server_context;

/**
*	\brief
*		Initializes the ATS TCP/IP Command server
*
*	\detdesc
*		Entry function to start ats_server threads
*		ats_server will start a listening thread on a local IP address on the
*		designated port.
*		ats communicates with the Gateway Server for any connection to external clients
*
*		Note: Only one client can be connected to this target at a time. The use of a mutex
*		ensures this.
*
*	\param [in] ats_cb: A callback to ats_exectue_command
*
*	\return
*		0 on success, non-zero on failure
*/
int32_t tcpip_cmd_server_init(void(*ats_cb)(uint8_t *buf, uint32_t buf_len, uint8_t **resp, uint32_t *resp_len));

/**
*   \brief
*		De-initializes the ATS TCP/IP server
*
*	\detdesc
*       Releases threading and memory resources aquired during tcpip_cmd_server_init
*
*		Note: Only one client can be connected to this target at a time. The use of a mutex
*		ensures this.
*
*	\return
*		0 on success, non-zero on failure
*/

/**
* \brief tcpip_cmd_server_deinit
*		Stops ats_server threads.
*
* \return 0 on success, non-zero on failure
*/
int32_t tcpip_cmd_server_deinit(void);

#ifdef FEATURE_ATS_PUSH
/**
	\brief
		Method to send payload data rettreived from ats service

	\detdesc
		Method will send on the port configured for  RTM, therefore uses the server socket object created
		by the Socket server for RTM.

	\dependencies
		There must be a socket already accepted

	\param [in] sendbuf: payload retreived from RTM Client
	  payload_size: Size of the payload
*/
int32_t tcpip_cmd_server_send_rtm_log_data(const uint8_t* sendbuf, uint32_t payload_size);
#endif /*FEATURE_ATS_PUSH*/

#endif /*ATS_TRANSPORT_TCPIP*/

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_TCPIP_CMD_SERVER_API_H_*/

