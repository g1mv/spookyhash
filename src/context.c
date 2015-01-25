/*
 * Centaurean SpookyHash
 *
 * Copyright (c) 2015, Guillaume Voirin
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Centaurean nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * 25/01/15 12:19
 *
 * ----------
 * SpookyHash
 * ----------
 *
 * Author(s)
 * Bob Jenkins (http://burtleburtle.net/bob/hash/spooky.html)
 *
 * Description
 * Very fast non cryptographic hash
 */

#include "context.h"

spookyhash_context* spookyhash_context_allocate(void *(*mem_alloc)(size_t)) {
    void *(*memory_alloc)(size_t) = mem_alloc == NULL ? malloc : mem_alloc;
    return (spookyhash_context *) memory_alloc(sizeof(spookyhash_context));
}

void spookyhash_context_free(spookyhash_context* context, void (*mem_free)(void *)) {
    void (*memory_free)(void *) = mem_free == NULL ? free : mem_free;
    memory_free(context);
}

void spookyhash_context_init(spookyhash_context *context, uint_fast64_t seed1, uint_fast64_t seed2) {
    context->m_length = 0;
    context->m_remainder = 0;
    context->m_state[0] = seed1;
    context->m_state[1] = seed2;
}
