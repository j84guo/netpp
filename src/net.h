/**
 * Todo:
 * - Figure out which objects can/cannot be copied/moved
 * - Re-think file descriptor verification/sharing... especially re: shutting
 *   down the connection in a timely manner.
 *
 * - Use a reader/writer interface instead of recv/send, that way buffered
 *   wrappers/scanners can easily be made?
 * - Socket options like non-blocking, timeout
 * - How to select/poll on multiple TCPConns
 */
#ifndef NET_H
#define NET_H

#include "utils.h"
#include "tcpconn.h"
#include "tcpserver.h"

#endif
