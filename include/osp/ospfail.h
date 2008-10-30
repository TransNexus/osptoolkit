/**************************************************************************
*** COPYRIGHT (c) 2002 by TransNexus, Inc.                              ***
***                                                                     ***
*** This software is property of TransNexus, Inc.                       ***
*** This software is freely available under license from TransNexus.    ***
*** The license terms and conditions for free use of this software by   ***
*** third parties are defined in the OSP Toolkit Software License       ***
*** Agreement (LICENSE.txt).  Any use of this software by third         ***
*** parties, which does not comply with the terms and conditions of the ***
*** OSP Toolkit Software License Agreement is prohibited without        ***
*** the prior, express, written consent of TransNexus, Inc.             ***
***                                                                     ***
*** Thank you for using the OSP ToolKit(TM).  Please report any bugs,   ***
*** suggestions or feedback to support@transnexus.com                   ***
***                                                                     ***
**************************************************************************/

/*
 * ospfail.h - Structures and prototypes for linked lists.
 */

#ifndef _OSPFAIL_H
#define _OSPFAIL_H

#include "osp/osp.h"

enum OSPEFAILREASON {
    /* no failure (call successful) 1 */
    OSPC_FAIL_NONE = 0,

    /* ISDN failures */
    OSPC_FAIL_UNALLOC_NUMBER = 1,
    OSPC_FAIL_NO_ROUTE_TO_NETWORK = 2,
    OSPC_FAIL_NO_ROUTE_TO_DEST = 3,
    OSPC_FAIL_SEND_SPECIAL_INFO_TONE = 4,
    OSPC_FAIL_MISDIALED_TRUNK_PREFIX = 5,
    OSPC_FAIL_CHANNEL_UNACCEPTABLE = 6,
    OSPC_FAIL_CALL_AWARDED = 7,
    OSPC_FAIL_PREEMTION = 8,
    OSPC_FAIL_CERCUIT_RESERVED_FOR_REUSE = 9,
    OSPC_FAIL_NORMAL_CALL_CLEARING = 1016,
    OSPC_FAIL_USER_BUSY = 17,
    OSPC_FAIL_NO_USER_RESPONDING = 18,
    OSPC_FAIL_NO_ANSWER_FROM_USER = 19,
    OSPC_FAIL_SUBSCRIBER_ABSENT = 20,
    OSPC_FAIL_CALL_REJECTED = 21,
    OSPC_FAIL_NUMBER_CHANGED = 22,
    OSPC_FAIL_NON_SELECTED_USER_CLEARING = 26,
    OSPC_FAIL_DEST_OUT_OF_ORDER = 27,
    OSPC_FAIL_INVALID_NUMBER_FORMAT = 28,
    OSPC_FAIL_FACILITY_REJECTED = 29,
    OSPC_FAIL_RESPONSE_TO_STATUS_ENQUIRY = 30,
    OSPC_FAIL_NORMAL_UNSPECIFIED = 31,
    OSPC_FAIL_NO_CHANNEL = 32,
    OSPC_FAIL_NETWORK_OUT_OF_ORDER = 38,
    OSPC_FAIL_PERMANENT_FRAME_MODE_CONNECTION_OUT_OF_SERVICE = 39,
    OSPC_FAIL_PERMANENT_FRAME_MODE_CONNECTION_OPERATIONAL = 40,
    OSPC_FAIL_TEMPORARY_FAILURE = 41,
    OSPC_FAIL_SWITCHING_EQUIPMENT_CONGESTION = 42,
    OSPC_FAIL_ACCESS_INFO_DISCARDED = 43,
    OSPC_FAIL_REQUESTED_CHANNEL_UNAVAIL = 44,
    OSPC_FAIL_PRECEDENCE_CALL_BLOCKED = 46,
    OSPC_FAIL_RESOURCE_UNAVAIL = 47,
    OSPC_FAIL_QUALITY_OF_SERVICE_UNAVAIL = 49,
    OSPC_FAIL_REQUESTED_FACILITY_NOT_SUBSCRIBED = 50,
    OSPC_FAIL_OUTGOING_CALLS_BARRED_WITH_CUG = 53,
    OSPC_FAIL_INCOMING_CALLS_BARRED_WITH_CUG = 55,
    OSPC_FAIL_BEARER_CAPABILITY_NO_AUTH = 57,
    OSPC_FAIL_BEARER_CAPABILITY_NO_AVAIL = 58,
    OSPC_FAIL_INCONSISTENCY_IN_OUTGOING_ACESS_INFO = 62,
    OSPC_FAIL_SERVICE_NOT_AVAILABLE = 63,
    OSPC_FAIL_BEARER_CAPABILITY_NOT_IMPLEMENTED = 65,
    OSPC_FAIL_CHANNEL_TYPE_NOT_IMPLEMENTED = 66,
    OSPC_FAIL_REQUESTED_FACILITY_NOT_IMPLEMENTED = 69,
    OSPC_FAIL_ONLY_RESTRICTED_DIGITAL_INFO_AVAIL = 70,
    OSPC_FAIL_SERVICE_NOT_IMPLEMENTED = 79,
    OSPC_FAIL_INVALID_CALL_REFERENCE_VALUE = 81,
    OSPC_FAIL_IDENTIFIED_CHANNEL_DOES_NOT_EXIST = 82,
    OSPC_FAIL_SUSPENDED_CALL_EXISTS_BUT_IDENTITY_DOES_NOT = 83,
    OSPC_FAIL_CALL_IDENTITY_IN_USE = 84,
    OSPC_FAIL_NO_CALL_SUSPENDED = 85,
    OSPC_FAIL_CALL_HAS_BEEN_CLEARED = 86,
    OSPC_FAIL_USER_NOT_MEMBEROF_CUG = 87,
    OSPC_FAIL_INCOMPATIBLE_DEST = 88,
    OSPC_FAIL_NON_EXISTENT_CUG = 90,
    OSPC_FAIL_INVALID_TRANSIT_NETWORK_SELECTION = 91,
    OSPC_FAIL_INVALID_MESSAGE = 95,
    OSPC_FAIL_MANDATORY_INFO_ELEMENT_MISSING = 96,
    OSPC_FAIL_MESSAGE_TYPE_NON_EXISTENT = 97,
    OSPC_FAIL_MESSAGE_NOT_COMPATIBLE_WITH_CALL_STATE_NOT_IMPLEMENTED = 98,
    OSPC_FAIL_INFO_ELEMENT_NON_EXISTENT = 99,
    OSPC_FAIL_INVALID_INFO_ELEMENT_CONTENTS = 100,
    OSPC_FAIL_MESSAGE_NOT_COMPATIBLE_WITH_CALL_STATE = 101,
    OSPC_FAIL_RECOVERY_ON_TIMER_EXPIRY = 102,
    OSPC_FAIL_PARAMETER_NON_EXISTENT_PASSED_ON = 103,
    OSPC_FAIL_MESSAGE_WITH_UNRECOGNIZED_PARAMETER_DISCARDED = 110,
    OSPC_FAIL_PROTOCOL_ERROR = 111,
    OSPC_FAIL_INTERWORKING = 127,

    /* generic failure */
    OSPC_FAIL_GENERAL = 999
};

/*
 * Will return success as long as ospvFailureReason is between
 * OSPC_FAIL_NONE (0) and OSPC_FAIL_GENERAL (999)
 */
int OSPPFailReasonFind(enum OSPEFAILREASON ospvFailureReason);

#endif /* _OSPFAIL_H */
