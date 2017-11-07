/*
 * This file is part of nmealib.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __NMEALIB_TEST_HELPERS_H_
#define __NMEALIB_TEST_HELPERS_H_

#include "mockContext.h"

#define validateContext(traces, errors) \
  {CU_ASSERT_EQUAL(nmeaTraceCalls, traces); \
   CU_ASSERT_EQUAL(nmeaErrorCalls, errors); \
   mockContextReset();}

#define validateParsePack(pack, r, rexp, traces, errors, empty) \
  {CU_ASSERT_EQUAL(r, rexp); \
   CU_ASSERT_EQUAL(nmeaTraceCalls, traces); \
   CU_ASSERT_EQUAL(nmeaErrorCalls, errors); \
   if (empty) { \
     CU_ASSERT_EQUAL(memcmp(pack, &packEmpty, sizeof(*pack)), 0); \
   } else { \
     CU_ASSERT_NOT_EQUAL(memcmp(pack, &packEmpty, sizeof(*pack)), 0); \
   } \
   mockContextReset();}

#define validatePackToInfo(info, traces, errors, empty) \
   {CU_ASSERT_EQUAL(nmeaTraceCalls, traces); \
    CU_ASSERT_EQUAL(nmeaErrorCalls, errors); \
    if (empty) { \
      CU_ASSERT_EQUAL(memcmp(info, &infoEmpty, sizeof(*info)), 0); \
    } else { \
      CU_ASSERT_NOT_EQUAL(memcmp(info, &infoEmpty, sizeof(*info)), 0); \
    } \
	mockContextReset();}

#define validateInfoToPack(pack, traces, errors, empty) \
   {CU_ASSERT_EQUAL(nmeaTraceCalls, traces); \
    CU_ASSERT_EQUAL(nmeaErrorCalls, errors); \
    if (empty) { \
      CU_ASSERT_EQUAL(memcmp(pack, &packEmpty, sizeof(*pack)), 0); \
    } else { \
      CU_ASSERT_NOT_EQUAL(memcmp(pack, &packEmpty, sizeof(*pack)), 0); \
    } \
	mockContextReset();}

#define checkSatellitesEmpty(satellites, istart, iend, v) \
    { \
      size_t i; \
      for (i = istart; i <= iend; i++) { \
        NmeaSatellite *satellite = &satellites[i]; \
        CU_ASSERT_EQUAL(satellite->prn, v); \
        CU_ASSERT_EQUAL(satellite->elevation, v); \
        CU_ASSERT_EQUAL(satellite->azimuth, v); \
        CU_ASSERT_EQUAL(satellite->snr, v); \
      } \
    }

#endif /* __NMEALIB_TEST_HELPERS_H_ */
