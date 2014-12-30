/*
 * This file is part of BVbase.
 *
 * BVbase is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * BVbase is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with BVbase; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef BVUTIL_THREADMESSAGE_H
#define BVUTIL_THREADMESSAGE_H

typedef struct AVThreadMessageQueue AVThreadMessageQueue;

typedef enum AVThreadMessageFlags {

    /**
     * Perform non-blocking operation.
     * If this flag is set, send and recv operations are non-blocking and
     * return BVERROR(EAGAIN) immediately if they can not proceed.
     */
    BV_THREAD_MESSAGE_NONBLOCK = 1,

} AVThreadMessageFlags;

/**
 * Allocate a new message queue.
 *
 * @param mq      pointer to the message queue
 * @param nelem   maximum number of elements in the queue
 * @param elsize  size of each element in the queue
 * @return  >=0 for success; <0 for error, in particular BVERROR(ENOSYS) if
 *          lavu was built without thread support
 */
int bv_thread_message_queue_alloc(AVThreadMessageQueue **mq,
                                  unsigned nelem,
                                  unsigned elsize);

/**
 * Free a message queue.
 *
 * The message queue must no longer be in use by another thread.
 */
void bv_thread_message_queue_free(AVThreadMessageQueue **mq);

/**
 * Send a message on the queue.
 */
int bv_thread_message_queue_send(AVThreadMessageQueue *mq,
                                 void *msg,
                                 unsigned flags);

/**
 * Receive a message from the queue.
 */
int bv_thread_message_queue_recv(AVThreadMessageQueue *mq,
                                 void *msg,
                                 unsigned flags);

/**
 * Set the sending error code.
 *
 * If the error code is set to non-zero, bv_thread_message_queue_recv() will
 * return it immediately when there are no longer available messages.
 * Conventional values, such as BVERROR_EOF or BVERROR(EAGAIN), can be used
 * to cause the receiving thread to stop or suspend its operation.
 */
void bv_thread_message_queue_set_err_send(AVThreadMessageQueue *mq,
                                          int err);

/**
 * Set the receiving error code.
 *
 * If the error code is set to non-zero, bv_thread_message_queue_send() will
 * return it immediately. Conventional values, such as BVERROR_EOF or
 * BVERROR(EAGAIN), can be used to cause the sending thread to stop or
 * suspend its operation.
 */
void bv_thread_message_queue_set_err_recv(AVThreadMessageQueue *mq,
                                          int err);

#endif /* BVUTIL_THREADMESSAGE_H */
