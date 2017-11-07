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

#include "testHelpers.h"

#include <nmealib/generator.h>
#include <CUnit/Basic.h>
#include <float.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int generatorSuiteSetup(void);

extern bool nmeaGeneratorInvokeNoise(NmeaGenerator *gen, NmeaInfo *info);

extern bool nmeaGeneratorInvokeStatic(NmeaGenerator *gen, NmeaInfo *info);
extern bool nmeaGeneratorResetStatic(NmeaGenerator *gen, NmeaInfo *info);
extern bool nmeaGeneratorInitStatic(NmeaGenerator *gen, NmeaInfo *info);

extern bool nmeaGeneratorInvokeRotate(NmeaGenerator *gen, NmeaInfo *info);
extern bool nmeaGeneratorResetRotate(NmeaGenerator *gen, NmeaInfo *info);
extern bool nmeaGeneratorInitRotate(NmeaGenerator *gen, NmeaInfo *info);

extern bool nmeaGeneratorInitRandomMove(NmeaGenerator *gen, NmeaInfo *info);
extern bool nmeaGeneratorInvokeRandomMove(NmeaGenerator *gen, NmeaInfo *info);

/*
 * Failing generator
 */

static bool failInit(NmeaGenerator *gen __attribute__((unused)), NmeaInfo *info __attribute__((unused))) {
  return false;
}

static bool failInvoke(NmeaGenerator *gen __attribute__((unused)), NmeaInfo *info __attribute__((unused))) {
  return false;
}

static bool failReset(NmeaGenerator *gen __attribute__((unused)), NmeaInfo *info __attribute__((unused))) {
  return false;
}

/*
 * Tests
 */

static void test_nmeaGeneratorInvokeNoise(void) {
  size_t it;
  NmeaInfo info;
  bool r;

  memset(&info, 0, sizeof(info));

  /* invalid inputs */

  r = nmeaGeneratorInvokeNoise(NULL, NULL);
  CU_ASSERT_EQUAL(r, false);

  /* normal */

  r = nmeaGeneratorInvokeNoise(NULL, &info);
  CU_ASSERT_EQUAL(r, true);
  CU_ASSERT_EQUAL(info.present,
      NMEALIB_INFO_PRESENT_MASK & ~(NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_UTCDATE | NMEALIB_PRESENT_UTCTIME));
  CU_ASSERT_EQUAL(info.smask, 0);
  CU_ASSERT_EQUAL(info.utc.year, 0);
  CU_ASSERT_EQUAL(info.utc.mon, 0);
  CU_ASSERT_EQUAL(info.utc.day, 0);
  CU_ASSERT_EQUAL(info.utc.hour, 0);
  CU_ASSERT_EQUAL(info.utc.min, 0);
  CU_ASSERT_EQUAL(info.utc.sec, 0);
  CU_ASSERT_EQUAL(info.utc.hsec, 0);
  CU_ASSERT_EQUAL(info.sig >= NMEALIB_SIG_FIX, true);
  CU_ASSERT_EQUAL(info.sig <= NMEALIB_SIG_SENSITIVE, true);
  CU_ASSERT_EQUAL(info.fix >= NMEALIB_FIX_2D, true);
  CU_ASSERT_EQUAL(info.fix <= NMEALIB_FIX_3D, true);
  CU_ASSERT_EQUAL(info.pdop >= 0.0, true);
  CU_ASSERT_EQUAL(info.pdop <= 9.0, true);
  CU_ASSERT_EQUAL(info.hdop >= 0.0, true);
  CU_ASSERT_EQUAL(info.hdop <= 9.0, true);
  CU_ASSERT_EQUAL(info.vdop >= 0.0, true);
  CU_ASSERT_EQUAL(info.vdop <= 9.0, true);
  CU_ASSERT_EQUAL(info.latitude >= 0.0, true);
  CU_ASSERT_EQUAL(info.latitude <= 100.0, true);
  CU_ASSERT_EQUAL(info.longitude >= 0.0, true);
  CU_ASSERT_EQUAL(info.longitude <= 100.0, true);
  CU_ASSERT_EQUAL(info.elevation >= -100.0, true);
  CU_ASSERT_EQUAL(info.elevation <= 100.0, true);
  CU_ASSERT_EQUAL(info.height >= -100.0, true);
  CU_ASSERT_EQUAL(info.height <= 100.0, true);
  CU_ASSERT_EQUAL(info.speed >= 0.0, true);
  CU_ASSERT_EQUAL(info.speed <= 100.0, true);
  CU_ASSERT_EQUAL(info.track >= 0.0, true);
  CU_ASSERT_EQUAL(info.track <= 360.0, true);
  CU_ASSERT_EQUAL(info.mtrack >= 0.0, true);
  CU_ASSERT_EQUAL(info.mtrack <= 360.0, true);
  CU_ASSERT_EQUAL(info.magvar >= 0.0, true);
  CU_ASSERT_EQUAL(info.magvar <= 360.0, true);
  CU_ASSERT_EQUAL(info.dgpsAge >= 0.0, true);
  CU_ASSERT_EQUAL(info.dgpsAge <= 100.0, true);
  CU_ASSERT_EQUAL(info.dgpsSid <= 100, true);

  CU_ASSERT_EQUAL(info.satellites.inUseCount <= NMEALIB_MAX_SATELLITES, true);
  CU_ASSERT_EQUAL(info.satellites.inViewCount <= NMEALIB_MAX_SATELLITES, true);

  for (it = 0; it < NMEALIB_MAX_SATELLITES; it++) {
    CU_ASSERT_EQUAL(info.satellites.inView[it].prn, it);
    CU_ASSERT_EQUAL(info.satellites.inView[it].elevation >= 0, true);
    CU_ASSERT_EQUAL(info.satellites.inView[it].elevation <= 90, true);
    CU_ASSERT_EQUAL(info.satellites.inView[it].azimuth <= 359, true);
    CU_ASSERT_EQUAL(info.satellites.inView[it].snr <= 99, true);
  }
}

static void test_nmeaGeneratorInvokeStatic(void) {
  NmeaInfo infoEmpty;
  NmeaInfo info;
  bool r;
  struct timeval tp;
  struct tm tt;

  memset(&infoEmpty, 0, sizeof(infoEmpty));
  memset(&info, 0, sizeof(info));

  /* invalid inputs */

  r = nmeaGeneratorInvokeStatic(NULL, NULL);
  CU_ASSERT_EQUAL(r, false);

  /* normal */

  gettimeofday(&tp, NULL);
  r = nmeaGeneratorInvokeStatic(NULL, &info);
  gmtime_r(&tp.tv_sec, &tt);
  CU_ASSERT_EQUAL(r, true);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_UTCDATE | NMEALIB_PRESENT_UTCTIME);
  CU_ASSERT_EQUAL(info.smask, 0);
  CU_ASSERT_EQUAL(info.utc.year, tt.tm_year + 1900);
  CU_ASSERT_EQUAL(info.utc.mon, tt.tm_mon + 1);
  CU_ASSERT_EQUAL(info.utc.day, tt.tm_mday);
  CU_ASSERT_EQUAL(info.utc.hour, tt.tm_hour);
  CU_ASSERT_EQUAL(info.utc.min, tt.tm_min);
  CU_ASSERT_EQUAL(info.utc.sec, tt.tm_sec);
  CU_ASSERT_EQUAL(info.sig, 0);
  CU_ASSERT_EQUAL(info.fix, 0);
  CU_ASSERT_DOUBLE_EQUAL(info.pdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.hdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.vdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.latitude, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.longitude, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.elevation, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.height, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.speed, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.track, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.mtrack, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.magvar, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.dgpsAge, 0.0, FLT_EPSILON);
  CU_ASSERT_EQUAL(info.dgpsSid, 0);

  CU_ASSERT_EQUAL(info.satellites.inUseCount, 0);
  CU_ASSERT_EQUAL(memcmp(info.satellites.inUse, infoEmpty.satellites.inUse, sizeof(info.satellites.inUse)), 0);
  CU_ASSERT_EQUAL(info.satellites.inViewCount, 0);
  CU_ASSERT_EQUAL(memcmp(info.satellites.inView, infoEmpty.satellites.inView, sizeof(info.satellites.inView)), 0);
}

static void test_nmeaGeneratorResetStatic(void) {
  NmeaInfo infoEmpty;
  NmeaInfo info;
  bool r;

  memset(&infoEmpty, 0, sizeof(infoEmpty));
  memset(&info, 0, sizeof(info));

  /* invalid inputs */

  r = nmeaGeneratorResetStatic(NULL, NULL);
  CU_ASSERT_EQUAL(r, false);

  /* normal */

  r = nmeaGeneratorResetStatic(NULL, &info);
  CU_ASSERT_EQUAL(r, true);
  CU_ASSERT_EQUAL(info.present,
      NMEALIB_PRESENT_SATINUSECOUNT | NMEALIB_PRESENT_SATINUSE | NMEALIB_PRESENT_SATINVIEWCOUNT
          | NMEALIB_PRESENT_SATINVIEW);
  CU_ASSERT_EQUAL(info.smask, 0);
  CU_ASSERT_EQUAL(info.utc.year, 0);
  CU_ASSERT_EQUAL(info.utc.mon, 0);
  CU_ASSERT_EQUAL(info.utc.day, 0);
  CU_ASSERT_EQUAL(info.utc.hour, 0);
  CU_ASSERT_EQUAL(info.utc.min, 0);
  CU_ASSERT_EQUAL(info.utc.sec, 0);
  CU_ASSERT_EQUAL(info.utc.hsec, 0);
  CU_ASSERT_EQUAL(info.sig, 0);
  CU_ASSERT_EQUAL(info.fix, 0);
  CU_ASSERT_DOUBLE_EQUAL(info.pdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.hdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.vdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.latitude, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.longitude, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.elevation, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.height, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.speed, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.track, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.mtrack, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.magvar, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.dgpsAge, 0.0, FLT_EPSILON);
  CU_ASSERT_EQUAL(info.dgpsSid, 0);

  infoEmpty.satellites.inUse[0] = 1;
  infoEmpty.satellites.inView[0].prn = 1;
  infoEmpty.satellites.inView[0].elevation = 50;
  infoEmpty.satellites.inView[0].azimuth = 0;
  infoEmpty.satellites.inView[0].snr = 99;

  infoEmpty.satellites.inUse[1] = 2;
  infoEmpty.satellites.inView[1].prn = 2;
  infoEmpty.satellites.inView[1].elevation = 50;
  infoEmpty.satellites.inView[1].azimuth = 90;
  infoEmpty.satellites.inView[1].snr = 99;

  infoEmpty.satellites.inUse[2] = 3;
  infoEmpty.satellites.inView[2].prn = 3;
  infoEmpty.satellites.inView[2].elevation = 50;
  infoEmpty.satellites.inView[2].azimuth = 180;
  infoEmpty.satellites.inView[2].snr = 99;

  infoEmpty.satellites.inUse[3] = 4;
  infoEmpty.satellites.inView[3].prn = 4;
  infoEmpty.satellites.inView[3].elevation = 50;
  infoEmpty.satellites.inView[3].azimuth = 270;
  infoEmpty.satellites.inView[3].snr = 99;

  CU_ASSERT_EQUAL(info.satellites.inUseCount, 4);
  CU_ASSERT_EQUAL(memcmp(info.satellites.inUse, infoEmpty.satellites.inUse, sizeof(info.satellites.inUse)), 0);
  CU_ASSERT_EQUAL(info.satellites.inViewCount, 4);
  CU_ASSERT_EQUAL(memcmp(info.satellites.inView, infoEmpty.satellites.inView, sizeof(info.satellites.inView)), 0);
}

static void test_nmeaGeneratorInitStatic(void) {
  NmeaGenerator gen;
  NmeaInfo infoEmpty;
  NmeaInfo info;
  bool r;

  memset(&infoEmpty, 0, sizeof(infoEmpty));
  memset(&info, 0, sizeof(info));

  /* invalid inputs */

  r = nmeaGeneratorInitStatic(NULL, NULL);
  CU_ASSERT_EQUAL(r, false);

  r = nmeaGeneratorInitStatic(&gen, NULL);
  CU_ASSERT_EQUAL(r, false);

  /* normal */

  r = nmeaGeneratorInitStatic(&gen, &info);
  CU_ASSERT_EQUAL(r, true);
  CU_ASSERT_EQUAL(nmeaInfoIsPresentAll(info.present, NMEALIB_PRESENT_SIG), true);
  CU_ASSERT_EQUAL(nmeaInfoIsPresentAll(info.present, NMEALIB_PRESENT_FIX), true);
  CU_ASSERT_EQUAL(info.sig, NMEALIB_SIG_SENSITIVE);
  CU_ASSERT_EQUAL(info.fix, NMEALIB_FIX_3D);
}

static void test_nmeaGeneratorInvokeRotate(void) {
  NmeaInfo infoEmpty;
  NmeaInfo info;
  bool r;
  struct timeval tp;
  struct tm tt;

  memset(&infoEmpty, 0, sizeof(infoEmpty));
  memset(&info, 0, sizeof(info));

  /* invalid inputs */

  r = nmeaGeneratorInvokeRotate(NULL, NULL);
  CU_ASSERT_EQUAL(r, false);

  /* normal, no sats */

  gettimeofday(&tp, NULL);
  r = nmeaGeneratorInvokeRotate(NULL, &info);
  gmtime_r(&tp.tv_sec, &tt);
  CU_ASSERT_EQUAL(r, true);
  CU_ASSERT_EQUAL(info.present,
      NMEALIB_PRESENT_UTCDATE | NMEALIB_PRESENT_UTCTIME | NMEALIB_PRESENT_SATINVIEWCOUNT | NMEALIB_PRESENT_SATINVIEW);
  CU_ASSERT_EQUAL(info.smask, 0);
  CU_ASSERT_EQUAL(info.utc.year, tt.tm_year + 1900);
  CU_ASSERT_EQUAL(info.utc.mon, tt.tm_mon + 1);
  CU_ASSERT_EQUAL(info.utc.day, tt.tm_mday);
  CU_ASSERT_EQUAL(info.utc.hour, tt.tm_hour);
  CU_ASSERT_EQUAL(info.utc.min, tt.tm_min);
  CU_ASSERT_EQUAL(info.utc.sec, tt.tm_sec);
  CU_ASSERT_EQUAL(info.sig, 0);
  CU_ASSERT_EQUAL(info.fix, 0);
  CU_ASSERT_DOUBLE_EQUAL(info.pdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.hdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.vdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.latitude, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.longitude, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.elevation, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.height, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.speed, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.track, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.mtrack, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.magvar, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.dgpsAge, 0.0, FLT_EPSILON);
  CU_ASSERT_EQUAL(info.dgpsSid, 0);

  CU_ASSERT_EQUAL(info.satellites.inUseCount, 0);
  CU_ASSERT_EQUAL(memcmp(info.satellites.inUse, infoEmpty.satellites.inUse, sizeof(info.satellites.inUse)), 0);
  CU_ASSERT_EQUAL(info.satellites.inViewCount, 0);
  CU_ASSERT_EQUAL(memcmp(info.satellites.inView, infoEmpty.satellites.inView, sizeof(info.satellites.inView)), 0);

  /* normal, 2 sats */

  info.satellites.inViewCount = 2;
  info.satellites.inView[0].prn = 1;
  info.satellites.inView[1].prn = 2;

  gettimeofday(&tp, NULL);
  r = nmeaGeneratorInvokeRotate(NULL, &info);
  gmtime_r(&tp.tv_sec, &tt);
  CU_ASSERT_EQUAL(r, true);
  CU_ASSERT_EQUAL(info.present,
      NMEALIB_PRESENT_UTCDATE | NMEALIB_PRESENT_UTCTIME | NMEALIB_PRESENT_SATINVIEWCOUNT | NMEALIB_PRESENT_SATINVIEW);
  CU_ASSERT_EQUAL(info.smask, 0);
  CU_ASSERT_EQUAL(info.utc.year, tt.tm_year + 1900);
  CU_ASSERT_EQUAL(info.utc.mon, tt.tm_mon + 1);
  CU_ASSERT_EQUAL(info.utc.day, tt.tm_mday);
  CU_ASSERT_EQUAL(info.utc.hour, tt.tm_hour);
  CU_ASSERT_EQUAL(info.utc.min, tt.tm_min);
  CU_ASSERT_EQUAL(info.utc.sec, tt.tm_sec);
  CU_ASSERT_EQUAL(info.sig, 0);
  CU_ASSERT_EQUAL(info.fix, 0);
  CU_ASSERT_DOUBLE_EQUAL(info.pdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.hdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.vdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.latitude, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.longitude, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.elevation, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.height, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.speed, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.track, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.mtrack, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.magvar, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.dgpsAge, 0.0, FLT_EPSILON);
  CU_ASSERT_EQUAL(info.dgpsSid, 0);

  infoEmpty.satellites.inView[0].prn = 1;
  infoEmpty.satellites.inView[0].azimuth = 5;
  infoEmpty.satellites.inView[1].prn = 2;
  infoEmpty.satellites.inView[1].azimuth = 185;

  CU_ASSERT_EQUAL(info.satellites.inUseCount, 0);
  CU_ASSERT_EQUAL(memcmp(info.satellites.inUse, infoEmpty.satellites.inUse, sizeof(info.satellites.inUse)), 0);
  CU_ASSERT_EQUAL(info.satellites.inViewCount, 2);
  CU_ASSERT_EQUAL(memcmp(info.satellites.inView, infoEmpty.satellites.inView, sizeof(info.satellites.inView)), 0);

  /* again */

  info.satellites.inViewCount = 2;
  info.satellites.inView[0].prn = 1;
  info.satellites.inView[1].prn = 2;
  info.satellites.inView[0].azimuth = 400;

  gettimeofday(&tp, NULL);
  r = nmeaGeneratorInvokeRotate(NULL, &info);
  gmtime_r(&tp.tv_sec, &tt);
  CU_ASSERT_EQUAL(r, true);
  CU_ASSERT_EQUAL(info.present,
      NMEALIB_PRESENT_UTCDATE | NMEALIB_PRESENT_UTCTIME | NMEALIB_PRESENT_SATINVIEWCOUNT | NMEALIB_PRESENT_SATINVIEW);
  CU_ASSERT_EQUAL(info.smask, 0);
  CU_ASSERT_EQUAL(info.utc.year, tt.tm_year + 1900);
  CU_ASSERT_EQUAL(info.utc.mon, tt.tm_mon + 1);
  CU_ASSERT_EQUAL(info.utc.day, tt.tm_mday);
  CU_ASSERT_EQUAL(info.utc.hour, tt.tm_hour);
  CU_ASSERT_EQUAL(info.utc.min, tt.tm_min);
  CU_ASSERT_EQUAL(info.utc.sec, tt.tm_sec);
  CU_ASSERT_EQUAL(info.sig, 0);
  CU_ASSERT_EQUAL(info.fix, 0);
  CU_ASSERT_DOUBLE_EQUAL(info.pdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.hdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.vdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.latitude, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.longitude, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.elevation, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.height, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.speed, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.track, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.mtrack, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.magvar, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.dgpsAge, 0.0, FLT_EPSILON);
  CU_ASSERT_EQUAL(info.dgpsSid, 0);

  infoEmpty.satellites.inView[0].prn = 1;
  infoEmpty.satellites.inView[0].azimuth = 45;
  infoEmpty.satellites.inView[1].prn = 2;
  infoEmpty.satellites.inView[1].azimuth = 225;

  CU_ASSERT_EQUAL(info.satellites.inUseCount, 0);
  CU_ASSERT_EQUAL(memcmp(info.satellites.inUse, infoEmpty.satellites.inUse, sizeof(info.satellites.inUse)), 0);
  CU_ASSERT_EQUAL(info.satellites.inViewCount, 2);
  CU_ASSERT_EQUAL(memcmp(info.satellites.inView, infoEmpty.satellites.inView, sizeof(info.satellites.inView)), 0);
}

static void test_nmeaGeneratorResetRotate(void) {
  NmeaGenerator gen;
  NmeaInfo info;
  bool r;

  memset(&info, 0, sizeof(info));

  /* invalid inputs */

  r = nmeaGeneratorResetRotate(NULL, NULL);
  CU_ASSERT_EQUAL(r, false);

  r = nmeaGeneratorResetRotate(&gen, NULL);
  CU_ASSERT_EQUAL(r, false);

  /* normal */

  r = nmeaGeneratorResetRotate(&gen, &info);
  CU_ASSERT_EQUAL(r, true);
  CU_ASSERT_EQUAL(info.present,
      NMEALIB_PRESENT_SATINUSECOUNT | NMEALIB_PRESENT_SATINUSE | NMEALIB_PRESENT_SATINVIEWCOUNT
          | NMEALIB_PRESENT_SATINVIEW);
  CU_ASSERT_EQUAL(info.smask, 0);
  CU_ASSERT_EQUAL(info.utc.year, 0);
  CU_ASSERT_EQUAL(info.utc.mon, 0);
  CU_ASSERT_EQUAL(info.utc.day, 0);
  CU_ASSERT_EQUAL(info.utc.hour, 0);
  CU_ASSERT_EQUAL(info.utc.min, 0);
  CU_ASSERT_EQUAL(info.utc.sec, 0);
  CU_ASSERT_EQUAL(info.utc.hsec, 0);
  CU_ASSERT_EQUAL(info.sig, 0);
  CU_ASSERT_EQUAL(info.fix, 0);
  CU_ASSERT_DOUBLE_EQUAL(info.pdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.hdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.vdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.latitude, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.longitude, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.elevation, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.height, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.speed, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.track, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.mtrack, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.magvar, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.dgpsAge, 0.0, FLT_EPSILON);
  CU_ASSERT_EQUAL(info.dgpsSid, 0);

  CU_ASSERT_EQUAL(info.satellites.inUseCount, 8);
  CU_ASSERT_EQUAL(info.satellites.inViewCount, 8);

  CU_ASSERT_EQUAL(info.satellites.inView[0].prn, 1);
  CU_ASSERT_EQUAL(info.satellites.inView[0].elevation, 5);
  CU_ASSERT_EQUAL(info.satellites.inView[0].azimuth, 0);
  CU_ASSERT_EQUAL(info.satellites.inView[0].snr, 80);

  CU_ASSERT_EQUAL(info.satellites.inView[1].prn, 2);
  CU_ASSERT_EQUAL(info.satellites.inView[1].elevation, 5);
  CU_ASSERT_EQUAL(info.satellites.inView[1].azimuth, 45);
  CU_ASSERT_EQUAL(info.satellites.inView[1].snr, 80);

  CU_ASSERT_EQUAL(info.satellites.inView[2].prn, 3);
  CU_ASSERT_EQUAL(info.satellites.inView[2].elevation, 5);
  CU_ASSERT_EQUAL(info.satellites.inView[2].azimuth, 90);
  CU_ASSERT_EQUAL(info.satellites.inView[2].snr, 80);

  CU_ASSERT_EQUAL(info.satellites.inView[3].prn, 4);
  CU_ASSERT_EQUAL(info.satellites.inView[3].elevation, 5);
  CU_ASSERT_EQUAL(info.satellites.inView[3].azimuth, 135);
  CU_ASSERT_EQUAL(info.satellites.inView[3].snr, 80);

  CU_ASSERT_EQUAL(info.satellites.inView[4].prn, 5);
  CU_ASSERT_EQUAL(info.satellites.inView[4].elevation, 5);
  CU_ASSERT_EQUAL(info.satellites.inView[4].azimuth, 180);
  CU_ASSERT_EQUAL(info.satellites.inView[4].snr, 80);

  CU_ASSERT_EQUAL(info.satellites.inView[5].prn, 6);
  CU_ASSERT_EQUAL(info.satellites.inView[5].elevation, 5);
  CU_ASSERT_EQUAL(info.satellites.inView[5].azimuth, 225);
  CU_ASSERT_EQUAL(info.satellites.inView[5].snr, 80);

  CU_ASSERT_EQUAL(info.satellites.inView[6].prn, 7);
  CU_ASSERT_EQUAL(info.satellites.inView[6].elevation, 5);
  CU_ASSERT_EQUAL(info.satellites.inView[6].azimuth, 270);
  CU_ASSERT_EQUAL(info.satellites.inView[6].snr, 80);

  CU_ASSERT_EQUAL(info.satellites.inView[7].prn, 8);
  CU_ASSERT_EQUAL(info.satellites.inView[7].elevation, 5);
  CU_ASSERT_EQUAL(info.satellites.inView[7].azimuth, 315);
  CU_ASSERT_EQUAL(info.satellites.inView[7].snr, 80);
}

static void test_nmeaGeneratorInitRotate(void) {
  NmeaGenerator gen;
  NmeaInfo info;
  bool r;

  memset(&info, 0, sizeof(info));

  /* invalid inputs */

  r = nmeaGeneratorInitRotate(NULL, NULL);
  CU_ASSERT_EQUAL(r, false);

  r = nmeaGeneratorInitRotate(&gen, NULL);
  CU_ASSERT_EQUAL(r, false);

  /* normal */

  r = nmeaGeneratorInitRotate(&gen, &info);
  CU_ASSERT_EQUAL(r, true);
  CU_ASSERT_EQUAL(nmeaInfoIsPresentAll(info.present, NMEALIB_PRESENT_SIG), true);
  CU_ASSERT_EQUAL(nmeaInfoIsPresentAll(info.present, NMEALIB_PRESENT_FIX), true);
  CU_ASSERT_EQUAL(info.sig, NMEALIB_SIG_SENSITIVE);
  CU_ASSERT_EQUAL(info.fix, NMEALIB_FIX_3D);
}

static void test_nmeaGeneratorInitRandomMove(void) {
  NmeaInfo infoEmpty;
  NmeaInfo info;
  bool r;

  memset(&infoEmpty, 0, sizeof(infoEmpty));
  memset(&info, 0, sizeof(info));

  /* invalid inputs */

  r = nmeaGeneratorInitRandomMove(NULL, NULL);
  CU_ASSERT_EQUAL(r, false);

  /* normal */

  r = nmeaGeneratorInitRandomMove(NULL, &info);
  CU_ASSERT_EQUAL(r, true);
  CU_ASSERT_EQUAL(info.present,
      NMEALIB_PRESENT_SIG | NMEALIB_PRESENT_FIX | NMEALIB_PRESENT_SPEED | NMEALIB_PRESENT_TRACK | NMEALIB_PRESENT_MTRACK
          | NMEALIB_PRESENT_MAGVAR);
  CU_ASSERT_EQUAL(info.smask, 0);
  CU_ASSERT_EQUAL(info.utc.year, 0);
  CU_ASSERT_EQUAL(info.utc.mon, 0);
  CU_ASSERT_EQUAL(info.utc.day, 0);
  CU_ASSERT_EQUAL(info.utc.hour, 0);
  CU_ASSERT_EQUAL(info.utc.min, 0);
  CU_ASSERT_EQUAL(info.utc.sec, 0);
  CU_ASSERT_EQUAL(info.utc.hsec, 0);
  CU_ASSERT_EQUAL(info.sig, NMEALIB_SIG_SENSITIVE);
  CU_ASSERT_EQUAL(info.fix, NMEALIB_FIX_3D);
  CU_ASSERT_DOUBLE_EQUAL(info.pdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.hdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.vdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.elevation, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.height, 0.0, FLT_EPSILON);

  CU_ASSERT_DOUBLE_EQUAL(info.speed, 20.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.track, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.mtrack, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.magvar, 0.0, FLT_EPSILON);

  CU_ASSERT_EQUAL(info.satellites.inUseCount, 0);
  CU_ASSERT_EQUAL(memcmp(info.satellites.inUse, infoEmpty.satellites.inUse, sizeof(info.satellites.inUse)), 0);
  CU_ASSERT_EQUAL(info.satellites.inViewCount, 0);
  CU_ASSERT_EQUAL(memcmp(info.satellites.inView, infoEmpty.satellites.inView, sizeof(info.satellites.inView)), 0);
}

static void test_nmeaGeneratorInvokeRandomMove(void) {
  NmeaInfo infoEmpty;
  NmeaInfo info;
  bool r;

  memset(&infoEmpty, 0, sizeof(infoEmpty));
  memset(&info, 0, sizeof(info));

  /* invalid inputs */

  r = nmeaGeneratorInvokeRandomMove(NULL, NULL);
  CU_ASSERT_EQUAL(r, false);

  /* normal */

  info.track = 100.0;
  info.mtrack = 100.0;
  info.speed = 20.0;
  r = nmeaGeneratorInvokeRandomMove(NULL, &info);
  CU_ASSERT_EQUAL(r, true);
  CU_ASSERT_EQUAL(info.present,
      NMEALIB_PRESENT_LAT | NMEALIB_PRESENT_LON | NMEALIB_PRESENT_SPEED | NMEALIB_PRESENT_TRACK
          | NMEALIB_PRESENT_MTRACK | NMEALIB_PRESENT_MAGVAR);
  CU_ASSERT_EQUAL(info.smask, 0);
  CU_ASSERT_EQUAL(info.utc.year, 0);
  CU_ASSERT_EQUAL(info.utc.mon, 0);
  CU_ASSERT_EQUAL(info.utc.day, 0);
  CU_ASSERT_EQUAL(info.utc.hour, 0);
  CU_ASSERT_EQUAL(info.utc.min, 0);
  CU_ASSERT_EQUAL(info.utc.sec, 0);
  CU_ASSERT_EQUAL(info.utc.hsec, 0);
  CU_ASSERT_EQUAL(info.sig, 0);
  CU_ASSERT_EQUAL(info.fix, 0);
  CU_ASSERT_DOUBLE_EQUAL(info.pdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.hdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.vdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.elevation, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.height, 0.0, FLT_EPSILON);
  CU_ASSERT_EQUAL(info.speed >= 1.0, true);
  CU_ASSERT_EQUAL(info.speed <= 40.0, true);
  CU_ASSERT_EQUAL((info.track >= 0.0) && (info.track < 360.0), true);
  CU_ASSERT_EQUAL((info.mtrack >= 0.0) && (info.mtrack < 360.0), true);
  CU_ASSERT_DOUBLE_EQUAL(info.magvar, info.track, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.dgpsAge, 0.0, FLT_EPSILON);
  CU_ASSERT_EQUAL(info.dgpsSid, 0);

  CU_ASSERT_EQUAL(info.satellites.inUseCount, 0);
  CU_ASSERT_EQUAL(memcmp(info.satellites.inUse, infoEmpty.satellites.inUse, sizeof(info.satellites.inUse)), 0);
  CU_ASSERT_EQUAL(info.satellites.inViewCount, 0);
  CU_ASSERT_EQUAL(memcmp(info.satellites.inView, infoEmpty.satellites.inView, sizeof(info.satellites.inView)), 0);

  /* underflows */

  info.track = -100.0;
  info.mtrack = -100.0;
  info.speed = -20.0;
  r = nmeaGeneratorInvokeRandomMove(NULL, &info);
  CU_ASSERT_EQUAL(r, true);
  CU_ASSERT_EQUAL(info.present,
      NMEALIB_PRESENT_LAT | NMEALIB_PRESENT_LON | NMEALIB_PRESENT_SPEED | NMEALIB_PRESENT_TRACK
          | NMEALIB_PRESENT_MTRACK | NMEALIB_PRESENT_MAGVAR);
  CU_ASSERT_EQUAL(info.smask, 0);
  CU_ASSERT_EQUAL(info.utc.year, 0);
  CU_ASSERT_EQUAL(info.utc.mon, 0);
  CU_ASSERT_EQUAL(info.utc.day, 0);
  CU_ASSERT_EQUAL(info.utc.hour, 0);
  CU_ASSERT_EQUAL(info.utc.min, 0);
  CU_ASSERT_EQUAL(info.utc.sec, 0);
  CU_ASSERT_EQUAL(info.utc.hsec, 0);
  CU_ASSERT_EQUAL(info.sig, 0);
  CU_ASSERT_EQUAL(info.fix, 0);
  CU_ASSERT_DOUBLE_EQUAL(info.pdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.hdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.vdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.elevation, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.height, 0.0, FLT_EPSILON);
  CU_ASSERT_EQUAL(info.speed >= 1.0, true);
  CU_ASSERT_EQUAL(info.speed <= 40.0, true);
  CU_ASSERT_EQUAL((info.track >= 0.0) && (info.track < 360.0), true);
  CU_ASSERT_EQUAL((info.mtrack >= 0.0) && (info.mtrack < 360.0), true);
  CU_ASSERT_DOUBLE_EQUAL(info.magvar, info.track, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.dgpsAge, 0.0, FLT_EPSILON);
  CU_ASSERT_EQUAL(info.dgpsSid, 0);

  /* overflows */

  info.track = 400.0;
  info.mtrack = 400.0;
  info.speed = 100.0;
  r = nmeaGeneratorInvokeRandomMove(NULL, &info);
  CU_ASSERT_EQUAL(r, true);
  CU_ASSERT_EQUAL(info.present,
      NMEALIB_PRESENT_LAT | NMEALIB_PRESENT_LON | NMEALIB_PRESENT_SPEED | NMEALIB_PRESENT_TRACK
          | NMEALIB_PRESENT_MTRACK | NMEALIB_PRESENT_MAGVAR);
  CU_ASSERT_EQUAL(info.smask, 0);
  CU_ASSERT_EQUAL(info.utc.year, 0);
  CU_ASSERT_EQUAL(info.utc.mon, 0);
  CU_ASSERT_EQUAL(info.utc.day, 0);
  CU_ASSERT_EQUAL(info.utc.hour, 0);
  CU_ASSERT_EQUAL(info.utc.min, 0);
  CU_ASSERT_EQUAL(info.utc.sec, 0);
  CU_ASSERT_EQUAL(info.utc.hsec, 0);
  CU_ASSERT_EQUAL(info.sig, 0);
  CU_ASSERT_EQUAL(info.fix, 0);
  CU_ASSERT_DOUBLE_EQUAL(info.pdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.hdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.vdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.elevation, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.height, 0.0, FLT_EPSILON);
  CU_ASSERT_EQUAL(info.speed >= 1.0, true);
  CU_ASSERT_EQUAL(info.speed <= 40.0, true);
  CU_ASSERT_EQUAL((info.track >= 0.0) && (info.track < 360.0), true);
  CU_ASSERT_EQUAL((info.mtrack >= 0.0) && (info.mtrack < 360.0), true);
  CU_ASSERT_DOUBLE_EQUAL(info.magvar, info.track, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.dgpsAge, 0.0, FLT_EPSILON);
  CU_ASSERT_EQUAL(info.dgpsSid, 0);

  CU_ASSERT_EQUAL(info.satellites.inUseCount, 0);
  CU_ASSERT_EQUAL(memcmp(info.satellites.inUse, infoEmpty.satellites.inUse, sizeof(info.satellites.inUse)), 0);
  CU_ASSERT_EQUAL(info.satellites.inViewCount, 0);
  CU_ASSERT_EQUAL(memcmp(info.satellites.inView, infoEmpty.satellites.inView, sizeof(info.satellites.inView)), 0);
}

static void test_nmeaGeneratorInit(void) {
  NmeaGenerator gen;
  NmeaGenerator *g;
  NmeaInfo infoEmpty;
  NmeaInfo info;
  bool r;
  struct timeval tp;
  struct tm tt;

  memset(&gen, 0, sizeof(gen));
  memset(&infoEmpty, 0, sizeof(infoEmpty));
  memset(&info, 0, sizeof(info));

  /* invalid inputs */

  r = nmeaGeneratorInit(NULL, NULL);
  CU_ASSERT_EQUAL(r, false);

  r = nmeaGeneratorInit(&gen, NULL);
  CU_ASSERT_EQUAL(r, false);

  /* normal, no calls */

  gettimeofday(&tp, NULL);
  r = nmeaGeneratorInit(&gen, &info);
  gmtime_r(&tp.tv_sec, &tt);
  CU_ASSERT_EQUAL(r, true);
  CU_ASSERT_EQUAL(nmeaInfoIsPresentAll(info.present, NMEALIB_PRESENT_SMASK), true);
  CU_ASSERT_EQUAL(nmeaInfoIsPresentAll(info.present, NMEALIB_PRESENT_LAT), true);
  CU_ASSERT_EQUAL(nmeaInfoIsPresentAll(info.present, NMEALIB_PRESENT_LON), true);
  CU_ASSERT_EQUAL(info.smask, 0);
  CU_ASSERT_EQUAL(info.utc.year, tt.tm_year + 1900);
  CU_ASSERT_EQUAL(info.utc.mon, tt.tm_mon + 1);
  CU_ASSERT_EQUAL(info.utc.day, tt.tm_mday);
  CU_ASSERT_EQUAL(info.utc.hour, tt.tm_hour);
  CU_ASSERT_EQUAL(info.utc.min, tt.tm_min);
  CU_ASSERT_EQUAL(info.utc.sec, tt.tm_sec);
  CU_ASSERT_EQUAL(info.sig, NMEALIB_SIG_INVALID);
  CU_ASSERT_EQUAL(info.fix, NMEALIB_FIX_BAD);
  CU_ASSERT_DOUBLE_EQUAL(info.pdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.hdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.vdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.elevation, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.height, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.speed, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.track, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.mtrack, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.magvar, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.dgpsAge, 0.0, FLT_EPSILON);
  CU_ASSERT_EQUAL(info.dgpsSid, 0);

  CU_ASSERT_EQUAL(info.satellites.inUseCount, 0);
  CU_ASSERT_EQUAL(memcmp(info.satellites.inUse, infoEmpty.satellites.inUse, sizeof(info.satellites.inUse)), 0);
  CU_ASSERT_EQUAL(info.satellites.inViewCount, 0);
  CU_ASSERT_EQUAL(memcmp(info.satellites.inView, infoEmpty.satellites.inView, sizeof(info.satellites.inView)), 0);

  /* normal, with calls */

  g = nmeaGeneratorCreate(NMEALIB_GENERATOR_STATIC, &info);
  CU_ASSERT_PTR_NOT_NULL_FATAL(g);

  gettimeofday(&tp, NULL);
  r = nmeaGeneratorInit(g, &info);
  gmtime_r(&tp.tv_sec, &tt);
  CU_ASSERT_EQUAL(r, true);
  CU_ASSERT_EQUAL(nmeaInfoIsPresentAll(info.present, NMEALIB_PRESENT_SMASK), true);
  CU_ASSERT_EQUAL(nmeaInfoIsPresentAll(info.present, NMEALIB_PRESENT_LAT), true);
  CU_ASSERT_EQUAL(nmeaInfoIsPresentAll(info.present, NMEALIB_PRESENT_LON), true);
  CU_ASSERT_EQUAL(info.smask, 0);
  CU_ASSERT_EQUAL(info.utc.year, tt.tm_year + 1900);
  CU_ASSERT_EQUAL(info.utc.mon, tt.tm_mon + 1);
  CU_ASSERT_EQUAL(info.utc.day, tt.tm_mday);
  CU_ASSERT_EQUAL(info.utc.hour, tt.tm_hour);
  CU_ASSERT_EQUAL(info.utc.min, tt.tm_min);
  CU_ASSERT_EQUAL(info.utc.sec, tt.tm_sec);
  CU_ASSERT_EQUAL(info.sig, NMEALIB_SIG_SENSITIVE);
  CU_ASSERT_EQUAL(info.fix, NMEALIB_FIX_3D);
  CU_ASSERT_DOUBLE_EQUAL(info.pdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.hdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.vdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.elevation, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.height, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.speed, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.track, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.mtrack, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.magvar, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.dgpsAge, 0.0, FLT_EPSILON);
  CU_ASSERT_EQUAL(info.dgpsSid, 0);

  infoEmpty.satellites.inUse[0] = 1;
  infoEmpty.satellites.inView[0].prn = 1;
  infoEmpty.satellites.inView[0].elevation = 50;
  infoEmpty.satellites.inView[0].azimuth = 0;
  infoEmpty.satellites.inView[0].snr = 99;

  infoEmpty.satellites.inUse[1] = 2;
  infoEmpty.satellites.inView[1].prn = 2;
  infoEmpty.satellites.inView[1].elevation = 50;
  infoEmpty.satellites.inView[1].azimuth = 90;
  infoEmpty.satellites.inView[1].snr = 99;

  infoEmpty.satellites.inUse[2] = 3;
  infoEmpty.satellites.inView[2].prn = 3;
  infoEmpty.satellites.inView[2].elevation = 50;
  infoEmpty.satellites.inView[2].azimuth = 180;
  infoEmpty.satellites.inView[2].snr = 99;

  infoEmpty.satellites.inUse[3] = 4;
  infoEmpty.satellites.inView[3].prn = 4;
  infoEmpty.satellites.inView[3].elevation = 50;
  infoEmpty.satellites.inView[3].azimuth = 270;
  infoEmpty.satellites.inView[3].snr = 99;

  CU_ASSERT_EQUAL(info.satellites.inUseCount, 4);
  CU_ASSERT_EQUAL(memcmp(info.satellites.inUse, infoEmpty.satellites.inUse, sizeof(info.satellites.inUse)), 0);
  CU_ASSERT_EQUAL(info.satellites.inViewCount, 4);
  CU_ASSERT_EQUAL(memcmp(info.satellites.inView, infoEmpty.satellites.inView, sizeof(info.satellites.inView)), 0);

  nmeaGeneratorDestroy(g);

  /* fail init */

  g = nmeaGeneratorCreate(NMEALIB_GENERATOR_STATIC, &info);
  CU_ASSERT_PTR_NOT_NULL_FATAL(g);
  g->init = failInit;

  r = nmeaGeneratorInit(g, &info);

  CU_ASSERT_EQUAL(r, false);

  nmeaGeneratorDestroy(g);
}

static void test_nmeaGeneratorCreate(void) {
  NmeaGenerator *gen;
  NmeaInfo info;

  /* invalid inputs */

  gen = nmeaGeneratorCreate(NMEALIB_GENERATOR_NOISE, NULL);
  CU_ASSERT_PTR_NULL(gen);

  gen = nmeaGeneratorCreate(NMEALIB_GENERATOR_LAST + 1, &info);
  CU_ASSERT_PTR_NULL(gen);

  /* noise */

  gen = nmeaGeneratorCreate(NMEALIB_GENERATOR_NOISE, &info);
  CU_ASSERT_PTR_NOT_NULL_FATAL(gen);
  CU_ASSERT_PTR_NULL(gen->init);
  CU_ASSERT_EQUAL(gen->invoke, nmeaGeneratorInvokeNoise);
  CU_ASSERT_PTR_NULL(gen->reset);
  CU_ASSERT_PTR_NULL(gen->next);
  nmeaGeneratorDestroy(gen);

  /* static */

  gen = nmeaGeneratorCreate(NMEALIB_GENERATOR_STATIC, &info);
  CU_ASSERT_PTR_NOT_NULL_FATAL(gen);
  CU_ASSERT_EQUAL(gen->init, nmeaGeneratorInitStatic);
  CU_ASSERT_EQUAL(gen->invoke, nmeaGeneratorInvokeStatic);
  CU_ASSERT_EQUAL(gen->reset, nmeaGeneratorResetStatic);
  CU_ASSERT_PTR_NULL(gen->next);
  nmeaGeneratorDestroy(gen);

  /* static sat */

  gen = nmeaGeneratorCreate(NMEALIB_GENERATOR_SAT_STATIC, &info);
  CU_ASSERT_PTR_NOT_NULL_FATAL(gen);
  CU_ASSERT_EQUAL(gen->init, nmeaGeneratorInitStatic);
  CU_ASSERT_EQUAL(gen->invoke, nmeaGeneratorInvokeStatic);
  CU_ASSERT_EQUAL(gen->reset, nmeaGeneratorResetStatic);
  CU_ASSERT_PTR_NULL(gen->next);
  nmeaGeneratorDestroy(gen);

  /* rotate sat */

  gen = nmeaGeneratorCreate(NMEALIB_GENERATOR_SAT_ROTATE, &info);
  CU_ASSERT_PTR_NOT_NULL_FATAL(gen);
  CU_ASSERT_EQUAL(gen->init, nmeaGeneratorInitRotate);
  CU_ASSERT_EQUAL(gen->invoke, nmeaGeneratorInvokeRotate);
  CU_ASSERT_EQUAL(gen->reset, nmeaGeneratorResetRotate);
  CU_ASSERT_PTR_NULL(gen->next);
  nmeaGeneratorDestroy(gen);

  /* random move */

  gen = nmeaGeneratorCreate(NMEALIB_GENERATOR_POS_RANDMOVE, &info);
  CU_ASSERT_PTR_NOT_NULL_FATAL(gen);
  CU_ASSERT_EQUAL(gen->init, nmeaGeneratorInitRandomMove);
  CU_ASSERT_EQUAL(gen->invoke, nmeaGeneratorInvokeRandomMove);
  CU_ASSERT_PTR_NULL(gen->reset);
  CU_ASSERT_PTR_NULL(gen->next);
  nmeaGeneratorDestroy(gen);

  /* rotate */

  gen = nmeaGeneratorCreate(NMEALIB_GENERATOR_ROTATE, &info);
  CU_ASSERT_PTR_NOT_NULL_FATAL(gen);
  CU_ASSERT_EQUAL(gen->init, nmeaGeneratorInitRotate);
  CU_ASSERT_EQUAL(gen->invoke, nmeaGeneratorInvokeRotate);
  CU_ASSERT_EQUAL(gen->reset, nmeaGeneratorResetRotate);
  CU_ASSERT_PTR_NOT_NULL(gen->next);
  CU_ASSERT_EQUAL(gen->next->init, nmeaGeneratorInitRandomMove);
  CU_ASSERT_EQUAL(gen->next->invoke, nmeaGeneratorInvokeRandomMove);
  CU_ASSERT_PTR_NULL(gen->next->reset);
  CU_ASSERT_PTR_NULL(gen->next->next);
  nmeaGeneratorDestroy(gen);
}

static void test_nmeaGeneratorReset(void) {
  bool r;
  NmeaGenerator gen;
  NmeaGenerator *g;
  NmeaInfo info;

  memset(&gen, 0, sizeof(gen));
  memset(&info, 0, sizeof(info));

  /* invalid inputs */

  r = nmeaGeneratorReset(NULL, NULL);
  CU_ASSERT_EQUAL(r, false);

  r = nmeaGeneratorReset(&gen, NULL);
  CU_ASSERT_EQUAL(r, false);

  /* normal, no reset call */

  g = nmeaGeneratorCreate(NMEALIB_GENERATOR_NOISE, &info);
  CU_ASSERT_PTR_NOT_NULL_FATAL(g);
  r = nmeaGeneratorReset(g, &info);
  CU_ASSERT_EQUAL(r, true);
  CU_ASSERT_EQUAL(info.satellites.inUseCount, 0);
  CU_ASSERT_EQUAL(info.satellites.inViewCount, 0);
  nmeaGeneratorDestroy(g);

  /* normal, reset call */

  g = nmeaGeneratorCreate(NMEALIB_GENERATOR_STATIC, &info);
  CU_ASSERT_PTR_NOT_NULL_FATAL(g);
  r = nmeaGeneratorReset(g, &info);
  CU_ASSERT_EQUAL(r, true);
  CU_ASSERT_EQUAL(info.satellites.inUseCount, 4);
  CU_ASSERT_EQUAL(info.satellites.inViewCount, 4);
  nmeaGeneratorDestroy(g);

  /* reset fail */

  g = nmeaGeneratorCreate(NMEALIB_GENERATOR_STATIC, &info);
  CU_ASSERT_PTR_NOT_NULL_FATAL(g);
  g->reset = failReset;
  r = nmeaGeneratorReset(g, &info);
  CU_ASSERT_EQUAL(r, false);
  nmeaGeneratorDestroy(g);
}

static void test_nmeaGeneratorDestroy(void) {
  NmeaGenerator *gen;
  NmeaInfo info;

  /* invalid inputs */

  nmeaGeneratorDestroy(NULL);

  /* normal */

  gen = nmeaGeneratorCreate(NMEALIB_GENERATOR_STATIC, &info);
  CU_ASSERT_PTR_NOT_NULL_FATAL(gen);
  nmeaGeneratorDestroy(gen);

  /* normal, 2 chained generators, no destroy */

  gen = nmeaGeneratorCreate(NMEALIB_GENERATOR_ROTATE, &info);
  CU_ASSERT_PTR_NOT_NULL_FATAL(gen);
  nmeaGeneratorDestroy(gen);
}

static void test_nmeaGeneratorInvoke(void) {
  NmeaInfo info;
  NmeaGenerator *gen = nmeaGeneratorCreate(NMEALIB_GENERATOR_STATIC, &info);
  bool r;

  CU_ASSERT_PTR_NOT_NULL_FATAL(gen);

  memset(&info, 0, sizeof(info));

  /* invalid inputs */

  r = nmeaGeneratorInvoke(NULL, NULL);
  CU_ASSERT_EQUAL(r, false);

  r = nmeaGeneratorInvoke(gen, NULL);
  CU_ASSERT_EQUAL(r, false);

  /* normal */

  r = nmeaGeneratorInvoke(gen, &info);
  CU_ASSERT_EQUAL(r, true);

  nmeaGeneratorDestroy(gen);

  /* normal, 2 chained generators */

  gen = nmeaGeneratorCreate(NMEALIB_GENERATOR_ROTATE, &info);
  CU_ASSERT_PTR_NOT_NULL_FATAL(gen);
  r = nmeaGeneratorInvoke(gen, &info);
  CU_ASSERT_EQUAL(r, true);

  nmeaGeneratorDestroy(gen);

  /* failed invoke */

  gen = nmeaGeneratorCreate(NMEALIB_GENERATOR_ROTATE, &info);
  gen->invoke = failInvoke;
  r = nmeaGeneratorInvoke(gen, &info);
  CU_ASSERT_EQUAL(r, false);

  nmeaGeneratorDestroy(gen);
}

static void test_nmeaGeneratorAppend(void) {
  NmeaInfo info;
  NmeaGenerator *gen = nmeaGeneratorCreate(NMEALIB_GENERATOR_STATIC, &info);
  NmeaGenerator *gen1 = nmeaGeneratorCreate(NMEALIB_GENERATOR_STATIC, &info);
  NmeaGenerator *gen2 = nmeaGeneratorCreate(NMEALIB_GENERATOR_ROTATE, &info);

  CU_ASSERT_PTR_NOT_NULL_FATAL(gen);
  CU_ASSERT_PTR_NOT_NULL_FATAL(gen1);
  CU_ASSERT_PTR_NOT_NULL_FATAL(gen2);

  memset(&info, 0, sizeof(info));

  /* invalid inputs */

  nmeaGeneratorAppend(NULL, NULL);

  nmeaGeneratorAppend(gen, NULL);

  /* self */

  nmeaGeneratorAppend(gen, gen);
  CU_ASSERT_PTR_NULL(gen->next);

  /* normal */

  nmeaGeneratorAppend(gen, gen1);
  CU_ASSERT_PTR_EQUAL(gen->next, gen1);
  CU_ASSERT_PTR_NULL(gen->next->next);

  nmeaGeneratorAppend(gen, gen1);
  CU_ASSERT_PTR_EQUAL(gen->next, gen1);
  CU_ASSERT_PTR_NULL(gen->next->next);

  nmeaGeneratorAppend(gen, gen2);
  nmeaGeneratorAppend(gen, gen1);
  nmeaGeneratorAppend(gen, gen);
  nmeaGeneratorAppend(gen, gen2);
  CU_ASSERT_PTR_EQUAL(gen->next, gen1);
  CU_ASSERT_PTR_EQUAL(gen->next->next, gen2);
  CU_ASSERT_PTR_EQUAL(gen->next->next->next, gen2->next);
  CU_ASSERT_PTR_NULL(gen->next->next->next->next);

  nmeaGeneratorDestroy(gen);
}

static void test_nmeaGeneratorGenerateFrom(void) {
  NmeaMallocedBuffer buf;
  NmeaInfo info;
  size_t r;

  NmeaGenerator *gen = nmeaGeneratorCreate(NMEALIB_GENERATOR_STATIC, &info);

  CU_ASSERT_PTR_NOT_NULL_FATAL(gen);

  memset(&buf, 0, sizeof(buf));
  memset(&info, 0, sizeof(info));

  /* invalid inputs */

  r = nmeaGeneratorGenerateFrom(NULL, &info, gen, 0);
  CU_ASSERT_EQUAL(r, 0);

  buf.buffer = (char *) &info;
  buf.bufferSize = 0;
  r = nmeaGeneratorGenerateFrom(&buf, &info, gen, 0);
  CU_ASSERT_EQUAL(r, 0);

  buf.buffer = NULL;
  buf.bufferSize = 1;
  r = nmeaGeneratorGenerateFrom(&buf, &info, gen, 0);
  CU_ASSERT_EQUAL(r, 0);

  buf.buffer = NULL;
  buf.bufferSize = 0;

  r = nmeaGeneratorGenerateFrom(&buf, NULL, gen, 0);
  CU_ASSERT_EQUAL(r, 0);

  r = nmeaGeneratorGenerateFrom(&buf, &info, NULL, 0);
  CU_ASSERT_EQUAL(r, 0);

  r = nmeaGeneratorGenerateFrom(&buf, &info, gen, 0);
  CU_ASSERT_EQUAL(r, 0);

  /* normal */

  r = nmeaGeneratorGenerateFrom(&buf, &info, gen, NMEALIB_SENTENCE_GPGSA);
  CU_ASSERT_EQUAL(r, 28);
  CU_ASSERT_PTR_NOT_NULL(buf.buffer);
  CU_ASSERT_EQUAL(buf.bufferSize, NMEALIB_BUFFER_CHUNK_SIZE);
  CU_ASSERT_STRING_EQUAL(buf.buffer, "$GPGSA,,,,,,,,,,,,,,,,,*6E\r\n");

  /* free the buffer */
  free(buf.buffer);
  buf.buffer = NULL;
  buf.bufferSize = 0;

  /* invoke fail */

  gen->invoke = failInvoke;
  r = nmeaGeneratorGenerateFrom(&buf, &info, gen, NMEALIB_SENTENCE_GPGSA);
  CU_ASSERT_EQUAL(r, 0);
  CU_ASSERT_PTR_NULL(buf.buffer);
  CU_ASSERT_EQUAL(buf.bufferSize, 0);

  nmeaGeneratorDestroy(gen);
}

/*
 * Setup
 */

int generatorSuiteSetup(void) {
  CU_pSuite pSuite = CU_add_suite("generator", mockContextSuiteInit, mockContextSuiteClean);
  if (!pSuite) {
    return CU_get_error();
  }

  if ( //
      (!CU_add_test(pSuite, "nmeaGeneratorInvokeNoise", test_nmeaGeneratorInvokeNoise)) //
      || (!CU_add_test(pSuite, "nmeaGeneratorInvokeStatic", test_nmeaGeneratorInvokeStatic)) //
      || (!CU_add_test(pSuite, "nmeaGeneratorResetStatic", test_nmeaGeneratorResetStatic)) //
      || (!CU_add_test(pSuite, "nmeaGeneratorInitStatic", test_nmeaGeneratorInitStatic)) //
      || (!CU_add_test(pSuite, "nmeaGeneratorInvokeRotate", test_nmeaGeneratorInvokeRotate)) //
      || (!CU_add_test(pSuite, "nmeaGeneratorResetRotate", test_nmeaGeneratorResetRotate)) //
      || (!CU_add_test(pSuite, "nmeaGeneratorInitRotate", test_nmeaGeneratorInitRotate)) //
      || (!CU_add_test(pSuite, "nmeaGeneratorInitRandomMove", test_nmeaGeneratorInitRandomMove)) //
      || (!CU_add_test(pSuite, "nmeaGeneratorInvokeRandomMove", test_nmeaGeneratorInvokeRandomMove)) //
      || (!CU_add_test(pSuite, "nmeaGeneratorInit", test_nmeaGeneratorInit)) //
      || (!CU_add_test(pSuite, "nmeaGeneratorCreate", test_nmeaGeneratorCreate)) //
      || (!CU_add_test(pSuite, "nmeaGeneratorReset", test_nmeaGeneratorReset)) //
      || (!CU_add_test(pSuite, "nmeaGeneratorDestroy", test_nmeaGeneratorDestroy)) //
      || (!CU_add_test(pSuite, "nmeaGeneratorInvoke", test_nmeaGeneratorInvoke)) //
      || (!CU_add_test(pSuite, "nmeaGeneratorAppend", test_nmeaGeneratorAppend)) //
      || (!CU_add_test(pSuite, "nmeaGeneratorGenerateFrom", test_nmeaGeneratorGenerateFrom)) //
      ) {
    return CU_get_error();
  }

  return CUE_SUCCESS;
}
