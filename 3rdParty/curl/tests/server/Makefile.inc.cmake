SET(noinst_PROGRAMS getpart resolve rtspd sockfilt sws tftpd fake_ntlm)

SET(CURLX_SRCS 
 ../../lib/mprintf.c 
 ../../lib/nonblock.c 
 ../../lib/strtoofft.c 
 ../../lib/timeval.c 
 ../../lib/warnless.c)

SET(CURLX_HDRS 
 ../../lib/curlx.h 
 ../../lib/nonblock.h 
 ../../lib/strtoofft.h 
 ../../lib/timeval.h 
 ../../lib/warnless.h)

SET(USEFUL 
 getpart.c 
 getpart.h 
 server_setup.h 
 ../../lib/base64.c 
 ../../lib/curl_base64.h 
 ../../lib/memdebug.c 
 ../../lib/memdebug.h)

SET(UTIL 
 util.c 
 util.h)

SET(getpart_SOURCES ${CURLX_SRCS} ${CURLX_HDRS} ${USEFUL} 
 testpart.c)
SET(getpart_LDADD ${CURL_NETWORK_AND_TIME_LIBS})
SET(getpart_CFLAGS ${AM_CFLAGS})

SET(resolve_SOURCES ${CURLX_SRCS} ${CURLX_HDRS} ${USEFUL} ${UTIL} 
 resolve.c)
SET(resolve_LDADD ${CURL_NETWORK_AND_TIME_LIBS})
SET(resolve_CFLAGS ${AM_CFLAGS})

SET(rtspd_SOURCES ${CURLX_SRCS} ${CURLX_HDRS} ${USEFUL} ${UTIL} 
 server_sockaddr.h 
 rtspd.c)
SET(rtspd_LDADD ${CURL_NETWORK_AND_TIME_LIBS})
SET(rtspd_CFLAGS ${AM_CFLAGS})

SET(sockfilt_SOURCES ${CURLX_SRCS} ${CURLX_HDRS} ${USEFUL} ${UTIL} 
 server_sockaddr.h 
 sockfilt.c 
 ../../lib/inet_pton.c)
SET(sockfilt_LDADD ${CURL_NETWORK_AND_TIME_LIBS})
SET(sockfilt_CFLAGS ${AM_CFLAGS})

SET(sws_SOURCES ${CURLX_SRCS} ${CURLX_HDRS} ${USEFUL} ${UTIL} 
 server_sockaddr.h 
 sws.c 
 ../../lib/inet_pton.c)
SET(sws_LDADD ${CURL_NETWORK_AND_TIME_LIBS})
SET(sws_CFLAGS ${AM_CFLAGS})

SET(tftpd_SOURCES ${CURLX_SRCS} ${CURLX_HDRS} ${USEFUL} ${UTIL} 
 server_sockaddr.h 
 tftpd.c 
 tftp.h)
SET(tftpd_LDADD ${CURL_NETWORK_AND_TIME_LIBS})
SET(tftpd_CFLAGS ${AM_CFLAGS})

SET(fake_ntlm_SOURCES ${CURLX_SRCS} ${CURLX_HDRS} ${USEFUL} ${UTIL} 
 fake_ntlm.c)
SET(fake_ntlm_LDADD ${CURL_NETWORK_AND_TIME_LIBS})
SET(fake_ntlm_CFLAGS ${AM_CFLAGS})
