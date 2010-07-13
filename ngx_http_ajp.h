
/*
 * Copyright (C) Weibin Yao(yaoweibin@gmail.com)
 */

/*Main source is from Apache's mod_ajp_proxy*/

/* Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file ajp.h
 * @brief Apache Jserv Protocol
 *
 * @defgroup AJP_defines AJP definitions
 * @ingroup  MOD_PROXY
 * @{
 */

#ifndef AJP_H
#define AJP_H

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include "ngx_http_ajp_header.h"
#include "ngx_http_ajp_module.h"

#define AJP13_DEF_HOST "127.0.0.1"
#ifdef NETWARE
#define AJP13_DEF_PORT 9009     /* default to 9009 since 8009 is used by OS */
#else
#define AJP13_DEF_PORT 8009
#endif

#define AJP13_HTTPS_INDICATOR           "HTTPS"
#define AJP13_SSL_CLIENT_CERT_INDICATOR "SSL_CLIENT_CERT"
#define AJP13_SSL_CIPHER_INDICATOR      "SSL_CIPHER"
#define AJP13_SSL_SESSION_INDICATOR     "SSL_SESSION_ID"
#define AJP13_SSL_KEY_SIZE_INDICATOR    "SSL_CIPHER_USEKEYSIZE"


#define APR_OS_START_USERERR 1000

/** AJP Specific error codes
 */
/** Buffer overflow exception */
#define AJP_EOVERFLOW           (APR_OS_START_USERERR + 1) 
/** Destination Buffer is to small */
#define AJP_ETOSMALL            (APR_OS_START_USERERR + 2) 
/** Invalid input parameters */
#define AJP_EINVAL              (APR_OS_START_USERERR + 3) 
/** Bad message signature */
#define AJP_EBAD_SIGNATURE      (APR_OS_START_USERERR + 4) 
/** Incoming message too bg */
#define AJP_ETOBIG              (APR_OS_START_USERERR + 5) 
/** Missing message header */
#define AJP_ENO_HEADER          (APR_OS_START_USERERR + 6) 
/** Bad message header */
#define AJP_EBAD_HEADER         (APR_OS_START_USERERR + 7) 
/** Bad message */
#define AJP_EBAD_MESSAGE        (APR_OS_START_USERERR + 8) 
/** Cant log via AJP14 */
#define AJP_ELOGFAIL            (APR_OS_START_USERERR + 9) 
/** Bad request method */
#define AJP_EBAD_METHOD         (APR_OS_START_USERERR + 10) 


/** A structure that represents ajp message */ 
typedef struct ajp_msg ajp_msg_t;

/** A structure that represents ajp message */ 
struct ajp_msg
{
    /** The buffer holding a AJP message */ 
    ngx_buf_t  *buf;
    /** The length of AJP message header (defaults to AJP_HEADER_LEN) */ 
    size_t  header_len;
    /** The length of AJP message */ 
    size_t  len;
    /** Flag indicating the origing of the message */ 
    int     server_side;
};

/**
 * Signature for the messages sent from Apache to tomcat
 */
#define AJP13_WS_HEADER             0x1234
#define AJP_HEADER_LEN              4
#define AJP_HEADER_SZ_LEN           2
#define AJP_HEADER_SZ               6
#define AJP_HEADER_SAVE_SZ          7
#define AJP_MSG_BUFFER_SZ           8192
#define AJP_MAX_BUFFER_SZ           65536
#define AJP13_MAX_SEND_BODY_SZ      (AJP_MAX_BUFFER_SZ - AJP_HEADER_SZ)
#define AJP_PING_PONG_SZ            128

/** Send a request from web server to container*/
#define CMD_AJP13_FORWARD_REQUEST   (unsigned char)2
/** Write a body chunk from the servlet container to the web server */
#define CMD_AJP13_SEND_BODY_CHUNK   (unsigned char)3
/** Send response headers from the servlet container to the web server. */
#define CMD_AJP13_SEND_HEADERS      (unsigned char)4
/** Marks the end of response. */
#define CMD_AJP13_END_RESPONSE      (unsigned char)5
/** Get further data from the web server if it hasn't all been transferred yet. */
#define CMD_AJP13_GET_BODY_CHUNK    (unsigned char)6
/** The web server asks the container to shut itself down. */
#define CMD_AJP13_SHUTDOWN          (unsigned char)7
/** Webserver ask container to take control (logon phase) */
#define CMD_AJP13_PING              (unsigned char)8
/** Container response to cping request */
#define CMD_AJP13_CPONG             (unsigned char)9
/** Webserver check if container is alive, since container should respond by cpong */
#define CMD_AJP13_CPING             (unsigned char)10

/** @} */

/**
 * @defgroup AJP_api AJP API functions
 * @{
 */
/**
 * Check a new AJP Message by looking at signature and return its size
 *
 * @param msg       AJP Message to check
 * @param len       Pointer to returned len
 * @return          NGX_OK or error
 */
ngx_int_t ajp_msg_check_header(ajp_msg_t *msg, size_t *len);

/**
 * Begin to parse an AJP Message, move the buffer header to the type's position.
 *
 * @param msg       AJP Message to parse
 * @return          NGX_OK or error
 */
ngx_int_t ajp_parse_begin(ajp_msg_t *msg);

/**
 * Reset an AJP Message
 *
 * @param msg       AJP Message to reset
 * @return          NGX_OK or error
 */
ngx_int_t ajp_msg_reset(ajp_msg_t *msg);

/**
 * Reuse an AJP Message
 *
 * @param msg       AJP Message to reuse
 * @return          NGX_OK or error
 */
ngx_int_t ajp_msg_reuse(ajp_msg_t *msg);

/**
 * Mark the end of an AJP Message
 *
 * @param msg       AJP Message to end
 * @return          NGX_OK or error
 */
ngx_int_t ajp_msg_end(ajp_msg_t *msg);

/**
 * Add an unsigned 32bits value to AJP Message
 *
 * @param msg       AJP Message to get value from
 * @param value     value to add to AJP Message
 * @return          NGX_OK or error
 */
ngx_int_t ajp_msg_append_uint32(ajp_msg_t *msg, uint32_t value);

/**
 * Add an unsigned 16bits value to AJP Message
 *
 * @param msg       AJP Message to get value from
 * @param value     value to add to AJP Message
 * @return          NGX_OK or error
 */
ngx_int_t ajp_msg_append_uint16(ajp_msg_t *msg, uint16_t value);

/**
 * Add an unsigned 8bits value to AJP Message
 *
 * @param msg       AJP Message to get value from
 * @param value     value to add to AJP Message
 * @return          NGX_OK or error
 */
ngx_int_t ajp_msg_append_uint8(ajp_msg_t *msg, u_char value);

/**
 *  Add a String in AJP message, and transform the String in ASCII 
 *  if convert is set and we're on an EBCDIC machine    
 *
 * @param msg       AJP Message to get value from
 * @param value     Pointer to String
 * @param convert   When set told to convert String to ASCII
 * @return          NGX_OK or error
 */
ngx_int_t ajp_msg_append_string_ex(ajp_msg_t *msg, ngx_str_t *value,
                                      int convert);
/**
 *  Add a String in AJP message, and transform 
 *  the String in ASCII if we're on an EBCDIC machine    
 */
#define ajp_msg_append_string(m, v) ajp_msg_append_string_ex(m, v, 1)

/**
 *  Add a String in AJP message. 
 */
#define ajp_msg_append_string_ascii(m, v) ajp_msg_append_string_ex(m, v, 0)

/**
 * Add a Byte array to AJP Message
 *
 * @param msg       AJP Message to get value from
 * @param value     Pointer to Byte array
 * @param valuelen  Byte array len
 * @return          NGX_OK or error
 */
ngx_int_t ajp_msg_append_bytes(ajp_msg_t *msg, const u_char *value,
                                  size_t valuelen);

/**
 * Get a 32bits unsigned value from AJP Message
 *
 * @param msg       AJP Message to get value from
 * @param rvalue    Pointer where value will be returned
 * @return          NGX_OK or error
 */
ngx_int_t ajp_msg_get_uint32(ajp_msg_t *msg, uint32_t *rvalue);

/**
 * Get a 16bits unsigned value from AJP Message
 *
 * @param msg       AJP Message to get value from
 * @param rvalue    Pointer where value will be returned
 * @return          NGX_OK or error
 */
ngx_int_t ajp_msg_get_uint16(ajp_msg_t *msg, uint16_t *rvalue);

/**
 * Peek a 16bits unsigned value from AJP Message, position in message
 * is not updated
 *
 * @param msg       AJP Message to get value from
 * @param rvalue    Pointer where value will be returned
 * @return          NGX_OK or error
 */
ngx_int_t ajp_msg_peek_uint16(ajp_msg_t *msg, uint16_t *rvalue);

/**
 * Get a 8bits unsigned value from AJP Message
 *
 * @param msg       AJP Message to get value from
 * @param rvalue    Pointer where value will be returned
 * @return          NGX_OK or error
 */
ngx_int_t ajp_msg_get_uint8(ajp_msg_t *msg, u_char *rvalue);

/**
 * Peek a 8bits unsigned value from AJP Message, position in message
 * is not updated
 *
 * @param msg       AJP Message to get value from
 * @param rvalue    Pointer where value will be returned
 * @return          NGX_OK or error
 */
ngx_int_t ajp_msg_peek_uint8(ajp_msg_t *msg, u_char *rvalue);

/**
 * Get a String value from AJP Message
 *
 * @param msg       AJP Message to get value from
 * @param rvalue    Pointer where value will be returned
 * @return          NGX_OK or error
 */
ngx_int_t ajp_msg_get_string(ajp_msg_t *msg, ngx_str_t *rvalue);


/**
 * Get a Byte array from AJP Message
 *
 * @param msg       AJP Message to get value from
 * @param rvalue    Pointer where value will be returned
 * @param rvalueLen Pointer where Byte array len will be returned
 * @return          NGX_OK or error
 */
ngx_int_t ajp_msg_get_bytes(ajp_msg_t *msg, u_char **rvalue,
                               size_t *rvalue_len);

/**
 * Create an AJP Message from pool
 *
 * @param pool      memory pool to allocate AJP message from
 * @param size      size of the buffer to create
 * @param rmsg      Pointer to newly created AJP message
 * @return          NGX_OK or error
 */
ngx_int_t ajp_msg_create(ngx_pool_t *pool, size_t size, ajp_msg_t **rmsg);

/**
 * Create an AJP Message's buffer from pool
 *
 * @param pool      memory pool to allocate AJP message from
 * @param size      size of the buffer to create
 * @param rmsg      Pointer to newly created AJP message
 * @return          NGX_OK or error
 */
ngx_int_t ajp_msg_create_buffer(ngx_pool_t *pool, size_t size, ajp_msg_t *msg);

/**
 * Create an AJP Message from pool without buffer
 *
 * @param pool      memory pool to allocate AJP message from
 * @param rmsg      Pointer to newly created AJP message
 * @return          NGX_OK or error
 */
ngx_int_t ajp_msg_create_without_buffer(ngx_pool_t *pool, ajp_msg_t **rmsg);

/**
 * Recopy an AJP Message to another
 *
 * @param smsg      source AJP message
 * @param dmsg      destination AJP message
 * @return          NGX_OK or error
 */
ngx_int_t ajp_msg_copy(ajp_msg_t *smsg, ajp_msg_t *dmsg);

/**
 * Serialize in an AJP Message a PING command
 *
 * +-----------------------+
 * | PING CMD (1 byte)     |
 * +-----------------------+
 *
 * @param smsg      AJP message to put serialized message
 * @return          NGX_OK or error
 */
ngx_int_t ajp_msg_serialize_ping(ajp_msg_t *msg);

/** 
 * Serialize in an AJP Message a CPING command
 *
 * +-----------------------+
 * | CPING CMD (1 byte)    |
 * +-----------------------+
 *
 * @param smsg      AJP message to put serialized message
 * @return          NGX_OK or error
 */
ngx_int_t ajp_msg_serialize_cping(ajp_msg_t *msg);

/**
 * Dump up to the first 1024 bytes on an AJP Message
 *
 * @param pool      pool to allocate from
 * @param msg       AJP Message to dump
 * @param err       error string to display
 * @return          dump message
 */
u_char * ajp_msg_dump(ngx_pool_t *pool, ajp_msg_t *msg, u_char *err);

/**
 * Fill the request packet into AJP message
 * @param msg       AJP message
 * @param r         current request
 * @param alcf      AJP configration structure
 * @return          NGX_OK or error
 */
ngx_int_t ajp_marshal_into_msgb(ajp_msg_t *msg,
        ngx_http_request_t *r, ngx_http_ajp_loc_conf_t *alcf);

/**
 * Allocate a msg to send data
 * @param pool      pool to allocate from
 * @param msg       returned AJP message
 * @return          NGX_OK or error
 */
ngx_int_t  ajp_alloc_data_msg(ngx_pool_t *pool, ajp_msg_t *msg);

/**
 * ending a msg to send data
 * @param msg       returned AJP message
 * @param size      size of the data to send
 * @return          NGX_OK or error
 */
ngx_int_t  ajp_data_msg_end(ajp_msg_t *msg, size_t len);

/**
 * Parse the message type 
 * @param r         current request
 * @param msg       AJP message
 * @return          AJP message type.
 */
int ajp_parse_type(ajp_msg_t *msg);

/**
 * Parse the header message from container 
 * @param r         current request
 * @param msg       AJP message
 * @param alcf      AJP configration structure
 * @return          NGX_OK or error
 */
ngx_int_t ajp_parse_header(ngx_http_request_t  *r, ngx_http_ajp_loc_conf_t *alcf, ajp_msg_t *msg);

/** 
 * Parse the message body and return data address and length 
 * @param r         current request
 * @param msg       AJP message
 * @param len       returned AJP message length 
 * @return          NGX_OK or error
 */
ngx_int_t  ajp_parse_data(ngx_http_request_t  *r, ajp_msg_t *msg, uint16_t *len);


/** 
 * Handle the CPING/CPONG messages
 * TODO
 */

#endif /* AJP_H */

