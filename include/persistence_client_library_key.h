#ifndef PERSISTENCE_CLIENT_LIBRARY_KEY_H
#define PERSISTENCE_CLIENT_LIBRARY_KEY_H

/******************************************************************************
 * Project         Persistency
 * (c) copyright   2011
 * Company         XS Embedded GmbH
 *****************************************************************************/
/******************************************************************************
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0. If a  copy of the MPL was not distributed
 * with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
******************************************************************************/
 /**
 * @file           persistence_client_library_key.h
 * @ingroup        Persistence client library
 * @author         Ingo Huerner (XSe) / Guy Sagnes (Continental)
 * @brief          Header of the persistence client library.
 *                 Library provides an API to access persistent data
 * @see            
 */
/** \ingroup SSW_PERS */
/** \defgroup SSW_PERS_KEYVALUE Client: Key-value access
 *  \{
 */
/** \defgroup SSW_PERS_KEYVALUE_INTERFACE API document
 *  \{
 */

#ifdef __cplusplus
extern "C" {
#endif


#define 	PERSIST_KEYVALUEAPI_INTERFACE_VERSION   (0x01030000U)



/**
 * @brief delete persistent data
 *
 * @param ldbid logical database ID
 * @param resource_id the resource ID
 * @param user_no  the user ID
 * @param seat_no  the seat number (seat 0 to 3)
 *
 * @return positive value: success; On error a negative value will be returned with th follwoing error codes:
 * EPERS_LOCKFS
 */
int key_delete(unsigned char ldbid, char* resource_id, unsigned char user_no, unsigned char seat_no);



/**
 * @brief gets the size of persistent data in bytes
 *
 * @param ldbid logical database ID
 * @param resource_id the resource ID
 * @param user_no  the user ID
 * @param seat_no  the seat number (seat 0 to 3)
 *
 * @return positive value: the size; On error a negative value will be returned with th follwoing error codes:
 * EPERS_LOCKFS, EPERS_BADPOL, EPERS_NOKEY, EPERS_NOKEYDATA or EPERS_NOPRCTABLE
 */
int key_get_size(unsigned char ldbid, char* resource_id, unsigned char user_no, unsigned char seat_no);

/**
 * @brief close the access to a key-value identified by key handle
 *
 * @param key_handle key value handle return by key_handle_open()
 *
 * @return positive value: success; On error a negative value will be returned with th follwoing error codes:
 * EPERS_LOCKFS
 */
int key_handle_close(int key_handle);



/**
 * @brief gets the size of persistent data in bytes identified by key handle
 *
 * @param key_handle key value handle return by key_handle_open()
 *
 * @return positive value: the size; On error a negative value will be returned with th follwoing error codes:
 */
int key_handle_get_size(int key_handle);



/**
 * @brief open a key-value
 *
 * @param ldbid logical database ID
 * @param resource_id the resource ID
 * @param user_no  the user ID
 * @param seat_no  the seat number (seat 0 to 3)
 *
 * @return positive value: the key handle to access the value;
 * On error a negative value will be returned with th follwoing error codes:
 */
int key_handle_open(unsigned char ldbid, char* resource_id, unsigned char user_no, unsigned char seat_no);



/**
 * @brief reads persistent data identified by key handle
 *
 * @param key_handle key value handle return by key_handle_open()
 * @param buffer the buffer for persistent data
 * @param buffer_size size of buffer for reading
 *
 * @return positive value: the bytes read; On error a negative value will be returned with th follwoing error codes:
 *
 */
int key_handle_read_data(int key_handle, unsigned char* buffer, int buffer_size);



/**
 * @brief register a change notification for persistent data
 *
 * @param key_handle key value handle return by key_handle_open()
 *
 * @return positive value: registration OK; On error a negative value will be returned with th follwoing error codes:
 */
int key_handle_register_notify_on_change(int key_handle);



/**
 * @brief writes persistent data identified by key handle
 *
 * @param key_handle key value handle return by key_handle_open()
 * @param buffer the buffer containing the persistent data to write
 * @param buffer_size the number of bytes to write (default max size is set to 16kB)
 *                    use environment variable PERS_MAX_KEY_VAL_DATA_SIZE to modify default size in bytes
 *
 * @return positive value: the bytes written; On error a negative value will be returned with th follwoing error codes:
 */
int key_handle_write_data(int key_handle, unsigned char* buffer, int buffer_size);



/**
 * @brief reads persistent data identified by ldbid and resource_id
 *
 * @param ldbid logical database ID
 * @param resource_id the resource ID
 * @param user_no  the user ID
 * @param seat_no  the seat number (seat 0 to 3)
 * @param buffer the buffer to read the persistent data
 * @param buffer_size size of buffer for reading
 *
 * @return positive value: the bytes read; On error a negative value will be returned with th follwoing error codes:
 */
int key_read_data(unsigned char ldbid, char* resource_id, unsigned char user_no, unsigned char seat_no, unsigned char* buffer, int buffer_size);



/**
 * @brief register a change notification for persistent data
 *
 * @param ldbid logical database ID of the resource to monitor
 * @param resource_id the resource ID
 * @param user_no  the user ID
 * @param seat_no  the seat number (seat 0 to 3)
 *
 * @return positive value: registration OK; On error a negative value will be returned with th follwoing error codes:
 */
int key_register_notify_on_change(unsigned char ldbid, char* resource_id, unsigned char user_no, unsigned char seat_no);



/**
 * @brief writes persistent data identified by ldbid and resource_id
 *
 * @param ldbid logical database ID
 * @param resource_id the resource ID
 * @param user_no  the user ID
 * @param seat_no  the seat number (seat 0 to 3)
 * @param buffer the buffer containing the persistent data to write
 * @param buffer_size the number of bytes to write (default max size is set to 16kB)
 *                    use environment variable PERS_MAX_KEY_VAL_DATA_SIZE to modify default size in bytes
 *
 * @return positive value: the bytes written; On error a negative value will be returned with th follwoing error codes:
 */
int key_write_data(unsigned char ldbid, char* resource_id, unsigned char user_no, unsigned char seat_no, unsigned char* buffer, int buffer_size);


#ifdef __cplusplus
}
#endif

/** \} */ /* End of API */
/** \} */ /* End of MODULE */

#endif /* PERSISTENCY_CLIENT_LIBRARY_KEY_H */

