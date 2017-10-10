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

#include <nmealib/parser.h>
#include <CUnit/Basic.h>
#include <stddef.h>
#include <stdlib.h>

int parserSuiteSetup(void);

extern void nmeaParserReset(NmeaParser * parser, NmeaParserSentenceState new_state);
extern bool nmeaParserIsHexCharacter(char c);
extern bool nmeaParserProcessCharacter(NmeaParser *parser, const char * c);

/*
 * Tests
 */

static void test_nmeaParserIsHexCharacter(void) {
  bool r;

  r = nmeaParserIsHexCharacter('\0');
  CU_ASSERT_EQUAL(r, false);

  r = nmeaParserIsHexCharacter(' ');
  CU_ASSERT_EQUAL(r, false);

  r = nmeaParserIsHexCharacter('0');
  CU_ASSERT_EQUAL(r, true);

  r = nmeaParserIsHexCharacter('1');
  CU_ASSERT_EQUAL(r, true);

  r = nmeaParserIsHexCharacter('2');
  CU_ASSERT_EQUAL(r, true);

  r = nmeaParserIsHexCharacter('3');
  CU_ASSERT_EQUAL(r, true);

  r = nmeaParserIsHexCharacter('4');
  CU_ASSERT_EQUAL(r, true);

  r = nmeaParserIsHexCharacter('5');
  CU_ASSERT_EQUAL(r, true);

  r = nmeaParserIsHexCharacter('6');
  CU_ASSERT_EQUAL(r, true);

  r = nmeaParserIsHexCharacter('7');
  CU_ASSERT_EQUAL(r, true);

  r = nmeaParserIsHexCharacter('8');
  CU_ASSERT_EQUAL(r, true);

  r = nmeaParserIsHexCharacter('9');
  CU_ASSERT_EQUAL(r, true);

  r = nmeaParserIsHexCharacter('a');
  CU_ASSERT_EQUAL(r, true);

  r = nmeaParserIsHexCharacter('b');
  CU_ASSERT_EQUAL(r, true);

  r = nmeaParserIsHexCharacter('c');
  CU_ASSERT_EQUAL(r, true);

  r = nmeaParserIsHexCharacter('d');
  CU_ASSERT_EQUAL(r, true);

  r = nmeaParserIsHexCharacter('e');
  CU_ASSERT_EQUAL(r, true);

  r = nmeaParserIsHexCharacter('f');
  CU_ASSERT_EQUAL(r, true);

  r = nmeaParserIsHexCharacter('A');
  CU_ASSERT_EQUAL(r, true);

  r = nmeaParserIsHexCharacter('B');
  CU_ASSERT_EQUAL(r, true);

  r = nmeaParserIsHexCharacter('C');
  CU_ASSERT_EQUAL(r, true);

  r = nmeaParserIsHexCharacter('D');
  CU_ASSERT_EQUAL(r, true);

  r = nmeaParserIsHexCharacter('E');
  CU_ASSERT_EQUAL(r, true);

  r = nmeaParserIsHexCharacter('F');
  CU_ASSERT_EQUAL(r, true);
}

static void test_nmeaParserReset(void) {
  NmeaParser parserEmpty;
  NmeaParser parser;

  memset(&parserEmpty, 0xff, sizeof(parserEmpty));
  memset(&parser, 0xff, sizeof(parser));

  /* invalid input */

  nmeaParserReset(NULL, 0);
  CU_ASSERT_EQUAL(memcmp(&parser, &parserEmpty, sizeof(parser)), 0);
  memset(&parser, 0, sizeof(parser));

  /* normal, without allocated buffer */

  memset(&parserEmpty, 0, sizeof(parserEmpty));
  parserEmpty.sentence.state = NMEALIB_SENTENCE_STATE_READ_CHECKSUM;

  nmeaParserReset(&parser, NMEALIB_SENTENCE_STATE_READ_CHECKSUM);
  CU_ASSERT_EQUAL(memcmp(&parser, &parserEmpty, sizeof(parser)), 0);
  memset(&parser, 0, sizeof(parser));

  /* normal, with allocated buffer */

  parser.bufferSize = NMEALIB_PARSER_SENTENCE_SIZE;
  parser.buffer = malloc(parser.bufferSize);
  CU_ASSERT_PTR_NOT_NULL_FATAL(parser.buffer);

  memset(&parserEmpty, 0, sizeof(parserEmpty));
  parserEmpty.bufferSize = parser.bufferSize;
  parserEmpty.buffer = parser.buffer;
  parserEmpty.sentence.state = NMEALIB_SENTENCE_STATE_READ_EOL;

  nmeaParserReset(&parser, NMEALIB_SENTENCE_STATE_READ_EOL);
  CU_ASSERT_EQUAL(memcmp(&parser, &parserEmpty, sizeof(parser)), 0);
  free(parser.buffer);
  memset(&parser, 0, sizeof(parser));
}

static void test_nmeaParserInit(void) {
  NmeaParser parser;
  bool r;

  memset(&parser, 0, sizeof(parser));

  /* invalid input */

  r = nmeaParserInit(NULL, 0);
  CU_ASSERT_EQUAL(r, false);
  memset(&parser, 0, sizeof(parser));

  /* normal */

  r = nmeaParserInit(&parser, 0);
  CU_ASSERT_EQUAL(r, true);
  CU_ASSERT_EQUAL(parser.sentence.state, NMEALIB_SENTENCE_STATE_SKIP_UNTIL_START);
  nmeaParserDestroy(&parser);
  memset(&parser, 0, sizeof(parser));
}

static void test_nmeaParserDestroy(void) {
  NmeaParser parser;
  bool r;

  memset(&parser, 0, sizeof(parser));

  /* invalid input */

  r = nmeaParserDestroy(NULL);
  CU_ASSERT_EQUAL(r, false);
  memset(&parser, 0, sizeof(parser));

  /* normal */

  r = nmeaParserDestroy(&parser);
  CU_ASSERT_EQUAL(r, true);
  CU_ASSERT_EQUAL(parser.sentence.state, NMEALIB_SENTENCE_STATE_SKIP_UNTIL_START);
  nmeaParserDestroy(&parser);
  memset(&parser, 0, sizeof(parser));
}

static void test_nmeaParserProcessCharacter(void) {
  NmeaParser parser;
  char c;
  bool r;
  size_t i;

  memset(&parser, 0, sizeof(parser));

  /* invalid inputs */

  r = nmeaParserProcessCharacter(NULL, NULL);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 0);

  r = nmeaParserProcessCharacter(&parser, NULL);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 0);

  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 0);

  nmeaParserInit(&parser, 0);

  /* keep looking for the start of a new sentence */

  c = '\0';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 0);

  c = 'G';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 0);

  /* start a new sentence */

  c = '$';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 1);

  /* completely fill read buffer */

  for (i = 0; i < NMEALIB_PARSER_SENTENCE_SIZE - 2; i++) {
    c = 'G';
    r = nmeaParserProcessCharacter(&parser, &c);
    CU_ASSERT_EQUAL(r, false);
    CU_ASSERT_EQUAL(parser.bufferLength, 2 + i);
  }

  c = 'G';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 0);

  /* keep looking for the start of a new sentence */

  c = 'G';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 0);

  /* start a new sentence */

  c = '$';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 1);

  /* continue new sentence */

  c = 'G';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 2);

  /* invalid character */

  c = '~';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 0);

  /* start a new sentence */

  c = '$';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 1);

  /* continue new sentence */

  c = 'G';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 2);

  /* start a new sentence */

  c = '$';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 1);

  /* continue new sentence */

  c = 'G';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 2);

  /* start checksum */

  c = '*';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 3);

  /* start a new sentence */

  c = '$';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 1);

  /* start checksum */

  c = '*';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 2);

  /* not a hex char */

  c = 'g';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 0);

  /* start a new sentence */

  c = '$';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 1);

  /* start checksum */

  c = '*';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 2);

  c = '1';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 3);

  c = '1';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 4);

  /* not an EOL */

  c = 'g';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 0);

  /* start a new sentence */

  c = '$';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 1);

  /* start checksum */

  c = '*';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 2);

  c = '1';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 3);

  c = '1';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 4);

  /* EOL 1 */

  c = '\r';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 5);

  /* not an EOL */

  c = 'g';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 0);

  /* start a new sentence */

  c = '$';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 1);

  /* start checksum */

  c = '*';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 2);

  c = '1';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 3);

  c = '1';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 4);

  /* EOL 1 */

  c = '\r';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 5);

  /* EOL 2 */

  c = '\n';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 4);
  CU_ASSERT_STRING_EQUAL(parser.buffer, "$*11");

  /* start a new sentence */

  c = '$';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 1);

  /* start checksum */

  c = '*';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 2);

  c = '0';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 3);

  c = '0';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 4);

  /* EOL 1 */

  c = '\r';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 5);

  /* EOL 2 */

  c = '\n';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, true);
  CU_ASSERT_EQUAL(parser.bufferLength, 4);
  CU_ASSERT_STRING_EQUAL(parser.buffer, "$*00");

  /* start a new sentence */

  c = '$';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 1);

  /* EOL 1 */

  c = '\r';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(parser.bufferLength, 2);

  /* EOL 2 */

  c = '\n';
  r = nmeaParserProcessCharacter(&parser, &c);
  CU_ASSERT_EQUAL(r, true);
  CU_ASSERT_EQUAL(parser.bufferLength, 1);
  CU_ASSERT_STRING_EQUAL(parser.buffer, "$");

  nmeaParserDestroy(&parser);
}

static void test_nmeaParserParse(void) {
  NmeaParser parser;
  NmeaInfo info;
  const char *s = "$GPGGA,,,,,,,,,,,,,,*56\r\n";
  size_t r;

  memset(&parser, 0, sizeof(parser));

  /* invalid inputs */

  r = nmeaParserParse(NULL, s, strlen(s), &info);
  CU_ASSERT_EQUAL(r, 0);

  r = nmeaParserParse(&parser, NULL, strlen(s), &info);
  CU_ASSERT_EQUAL(r, 0);

  r = nmeaParserParse(&parser, s, 0, &info);
  CU_ASSERT_EQUAL(r, 0);

  r = nmeaParserParse(&parser, s, strlen(s), NULL);
  CU_ASSERT_EQUAL(r, 0);

  r = nmeaParserParse(&parser, s, strlen(s), &info);
  CU_ASSERT_EQUAL(r, 0);

  nmeaParserInit(&parser, 0);

  /* parse */

  s = "$GPGGA,,,,,,,,,,,,,,*00\r\n";
  r = nmeaParserParse(&parser, s, strlen(s), &info);
  CU_ASSERT_EQUAL(r, 0);

  s = "$GPGGA,,,,,,,,,,,,,,*56\r\n";
  r = nmeaParserParse(&parser, s, strlen(s), &info);
  CU_ASSERT_EQUAL(r, 1);

  s = "$GPGGA,,,,,,,,,,,,,,*56\r\n$GPGGA,,,,,,,,,,,,,,*56\r\n$GPGGA,,,,,,,,,,,,,,*56\r\n";
  r = nmeaParserParse(&parser, s, strlen(s), &info);
  CU_ASSERT_EQUAL(r, 3);

  s = "$GPGGA,,,,,,,,,,,,,,*56\r\n$GPGGA,,,,,,$,,,,,,,,*56\r\n$GPGGA,,,,,,,,,,,,,,*56\r\n";
  r = nmeaParserParse(&parser, s, strlen(s), &info);
  CU_ASSERT_EQUAL(r, 2);

  nmeaParserDestroy(&parser);
}

/*
 * Setup
 */

int parserSuiteSetup(void) {
  CU_pSuite pSuite = CU_add_suite("parser", mockContextSuiteInit, mockContextSuiteClean);
  if (!pSuite) {
    return CU_get_error();
  }

  if ( //
      (!CU_add_test(pSuite, "nmeaParserIsHexCharacter", test_nmeaParserIsHexCharacter)) //
      || (!CU_add_test(pSuite, "nmeaParserReset", test_nmeaParserReset)) //
      || (!CU_add_test(pSuite, "nmeaParserInit", test_nmeaParserInit)) //
      || (!CU_add_test(pSuite, "nmeaParserDestroy", test_nmeaParserDestroy)) //
      || (!CU_add_test(pSuite, "nmeaParserProcessCharacter", test_nmeaParserProcessCharacter)) //
      || (!CU_add_test(pSuite, "nmeaParserParse", test_nmeaParserParse)) //
      ) {
    return CU_get_error();
  }

  return CUE_SUCCESS;
}
