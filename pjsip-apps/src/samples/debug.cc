/* $Id: debug.cpp 4704 2014-01-16 05:30:46Z ming $ */
/* 
 * Copyright (C) 2008-2011 Teluu Inc. (http://www.teluu.com)
 * Copyright (C) 2003-2008 Benny Prijono <benny@prijono.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */

/*
 * PURPOSE:
 *   The purpose of this file is to allow debugging of a sample application
 *   using MSVC IDE.
 */

/* To debug a sample application, include the source file here.
 * E.g.:
 *  #include "playfile.c"
 */
#if 1
//#include "vid_codec_test.c"
#else
#include <pjlib.h>
#include <pjmedia/endpoint.h>
#include <pjmedia/transport.h>
#include <pjmedia/transport_udp.h>
#include <pjmedia/transport_srtp.h>

#include <string>
#include <iostream>

//#include <>

#define THIS_FILE "[debug]"

void LoopInput() {
  std::string input;
  do {
    std::cin >> input;
  } while (input != "Q" && input != "q");
}

int main(int argc, char **argv) {
  char szError[1024];

  int ret = pj_init();
  if (ret != PJ_SUCCESS) {
    pj_strerror(ret, szError, 1024);
    PJ_LOG(2, (THIS_FILE, "pj_init() failed. %s", szError));
    return ret;
  }

  pj_caching_pool cp;
  pj_caching_pool_init(&cp, NULL, 0);

  pjmedia_endpt *media_endpt;
  //1. Create pjmedia_endpt
  ret = pjmedia_endpt_create2(&cp.factory, NULL, 1, &media_endpt);
  if (ret != PJ_SUCCESS) {
    pj_strerror(ret, szError, 1024);
    PJ_LOG(2, (THIS_FILE, "pjmedia_endpt_create2() failed. %s", szError));
    return ret;
  }
  pjmedia_transport *udp_tp;
  ret = pjmedia_transport_udp_create(media_endpt, "udp_port_33333", 33333, 0, &udp_tp);
  if (ret != PJ_SUCCESS) {
    pj_strerror(ret, szError, 1024);
    PJ_LOG(2, (THIS_FILE, "pjmedia_transport_udp_create() port: 123456 failed. %s", szError));
    return ret;
  }
 
  //Create an Offer
  pj_pool_t *sdp_pool = pjmedia_endpt_create_pool(media_endpt, "sdp_pool", 400, 400);

  pjmedia_sock_info sock_info[2];

  pjmedia_transport_info udp_tp_info;
  pjmedia_transport_get_info(udp_tp, &udp_tp_info);

  //sock_info[0] = udp_tp_info.sock_info;
  pj_memcpy(&sock_info[0], &udp_tp_info.sock_info, sizeof(pjmedia_sock_info));

  pjmedia_sdp_session *sdp_session;
  //pjmedia_endpt_create_sdp()
  pjmedia_endpt_create_sdp(media_endpt, sdp_pool, 1, sock_info, &sdp_session);


  ret = pjmedia_transport_encode_sdp(udp_tp, sdp_pool, sdp_session, NULL, 0);
  if (ret != PJ_SUCCESS) {
    pj_strerror(ret, szError, 1024);
    PJ_LOG(2, (THIS_FILE, "pjmedia_transport_encode_sdp() failed. %s", szError));
    return ret;
  }

  LoopInput();

  pjmedia_transport_close(udp_tp);
  //pjmedia_transport *tp_udp = pjmedia_transport_udp_create();
  pjmedia_endpt_destroy2(media_endpt);
  media_endpt = NULL;

  pj_caching_pool_destroy(&cp);
  pj_shutdown();
  return 0;
}
#endif
