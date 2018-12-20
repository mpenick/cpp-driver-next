/*
  Copyright (c) DataStax, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef DATASTAX_HPP
#define DATASTAX_HPP

typedef struct DsResult_ DsResultSet;
typedef struct DsValueDefinition_ DsValueDefinition;

#define DS_VALUE_TYPE_MAPPING(XX) \
  XX(DS_VALUE_TYPE_CUSTOM,  0x0000, "", "") \
  XX(DS_VALUE_TYPE_ASCII,  0x0001, "ascii", "org.apache.cassandra.db.marshal.AsciiType") \
  XX(DS_VALUE_TYPE_BIGINT,  0x0002, "bigint", "org.apache.cassandra.db.marshal.LongType") \
  XX(DS_VALUE_TYPE_BLOB,  0x0003, "blob", "org.apache.cassandra.db.marshal.BytesType") \
  XX(DS_VALUE_TYPE_BOOLEAN,  0x0004, "boolean", "org.apache.cassandra.db.marshal.BooleanType") \
  XX(DS_VALUE_TYPE_COUNTER,  0x0005, "counter", "org.apache.cassandra.db.marshal.CounterColumnType") \
  XX(DS_VALUE_TYPE_DECIMAL,  0x0006, "decimal", "org.apache.cassandra.db.marshal.DecimalType") \
  XX(DS_VALUE_TYPE_DOUBLE,  0x0007, "double", "org.apache.cassandra.db.marshal.DoubleType") \
  XX(DS_VALUE_TYPE_FLOAT,  0x0008, "float", "org.apache.cassandra.db.marshal.FloatType") \
  XX(DS_VALUE_TYPE_INT,  0x0009, "int", "org.apache.cassandra.db.marshal.Int32Type") \
  XX(DS_VALUE_TYPE_TEXT,  0x000A, "text", "org.apache.cassandra.db.marshal.UTF8Type") \
  XX(DS_VALUE_TYPE_TIMESTAMP,  0x000B, "timestamp", "org.apache.cassandra.db.marshal.TimestampType") \
  XX(DS_VALUE_TYPE_UUID,  0x000C, "uuid", "org.apache.cassandra.db.marshal.UUIDType") \
  XX(DS_VALUE_TYPE_VARCHAR,  0x000D, "varchar", "") \
  XX(DS_VALUE_TYPE_VARINT,  0x000E, "varint", "org.apache.cassandra.db.marshal.IntegerType") \
  XX(DS_VALUE_TYPE_TIMEUUID,  0x000F, "timeuuid", "org.apache.cassandra.db.marshal.TimeUUIDType") \
  XX(DS_VALUE_TYPE_INET,  0x0010, "inet", "org.apache.cassandra.db.marshal.InetAddressType") \
  XX(DS_VALUE_TYPE_DATE,  0x0011, "date", "org.apache.cassandra.db.marshal.SimpleDateType") \
  XX(DS_VALUE_TYPE_TIME,  0x0012, "time", "org.apache.cassandra.db.marshal.TimeType") \
  XX(DS_VALUE_TYPE_SMALL_INT,  0x0013, "smallint", "org.apache.cassandra.db.marshal.ShortType") \
  XX(DS_VALUE_TYPE_TINY_INT,  0x0014, "tinyint", "org.apache.cassandra.db.marshal.ByteType") \
  XX(DS_VALUE_TYPE_DURATION,  0x0015, "duration", "org.apache.cassandra.db.marshal.DurationType") \
  XX(DS_VALUE_TYPE_LIST,  0x0020, "list", "org.apache.cassandra.db.marshal.ListType") \
  XX(DS_VALUE_TYPE_MAP,  0x0021, "map", "org.apache.cassandra.db.marshal.MapType") \
  XX(DS_VALUE_TYPE_SET,  0x0022, "set", "org.apache.cassandra.db.marshal.SetType") \
  XX(DS_VALUE_TYPE_UDT,  0x0030, "", "") \
  XX(DS_VALUE_TYPE_TUPLE,  0x0031, "tuple", "org.apache.cassandra.db.marshal.TupleType")

typedef enum DsValueType_ {
  DS_VALUE_TYPE_UNKNOWN = 0xFFFF,
#define XX_VALUE_TYPE(name, type, cql, klass) name = type,
  DS_VALUE_TYPE_MAPPING(XX_VALUE_TYPE)
#undef XX_VALUE_TYPE
  /* @cond IGNORE */
  DS_VALUE_TYPE_LAST_ENTRY
  /* @endcond */
} DsValueType;

typedef struct DsUuid_ {
  /**
   * Represents the time and version part of a UUID. The most significant
   * 4 bits represent the version and the bottom 60 bits representing the
   * time part. For version 1 the time part represents the number of
   * 100 nanosecond periods since 00:00:00 UTC, January 1, 1970 (the Epoch).
   * For version 4 the time part is randomly generated.
   */
  uint64_t time_and_version;
  /**
   * Represents the clock sequence and the node part of a UUID. The most
   * significant 16 bits represent the clock sequence (except for the most
   * significant bit which is always set) and the bottom 48 bits represent
   * the node part. For version 1 (time-based) the clock sequence part is randomly
   * generated and the node part can be explicitly set, otherwise, it's generated
   * from node unique information. For version 4 both the clock sequence and the node
   * parts are randomly generated.
   */
  uint64_t clock_seq_and_node;
} DsUuid;

/**
 * The size of a hexadecimal UUID string including a null terminator.
 */
#define DS_UUID_STRING_LENGTH 37

typedef struct DsRow_ {
  const DsResultSet* result;
  const void* position;
} DsRow;

typedef struct DsValue_ {
  const DsResultSet* result;
  const DsValueDefinition* definition;
  const void* position;
  const void* end;
} DsValue;

/**
 * Convert a UUID to a string
 */
void ds_uuid_string(DsUuid uuid, char* output);

/**
 * Initialize row at the beginning of the result set.
 *
 * Note: This gets the first row. :)
 */
DsRow ds_row_init(const DsResultSet* result);

/**
 * Initialize row at the specified index in the result set. An invalid row is
 * returned if the index is outside of the result set.
 */
DsRow ds_row_init_at(const DsResultSet* result, size_t index);

/**
 * Move to the next row. Only call with a valid row.
 */
void ds_row_next(DsRow* row);

/**
 * Is the row valid. A row is invalid if at the end of the result set or if and
 * invalid index was passed to `ds_row_init_at()`.
 */
bool ds_row_is_valid(const DsRow* row);

/**
 * Get the column value at the specified column index. An invalid value is
 * returned if the index is outside the row.
 */
DsValue ds_row_column(const DsRow* row, size_t index);

/**
 * Get the column value at the specified column name. An invalid value is
 * returned if the name isn't a valid column.
 */
DsValue ds_row_column_by_name(const DsRow* row, const char* name);

/**
 * Same as `ds_row_column_by_name()`.
 */
DsValue ds_row_column_by_name_n(const DsRow* row, const char* name, size_t name_length);

/**
 * Initialize value starting with the first value of the first row of the result
 * set.
 */
DsValue ds_value_init(const DsResultSet* result);

/**
 * Initialize value starting at the first value of the specified row.
 */
DsValue ds_value_init_at(const DsRow* row);

/**
 * Initialize value starting at the first element of the specified collection.
 * An invalid value is returned if the specified value isn't a collection.
 */
DsValue ds_value_init_in(const DsValue* collection);

/**
 * Move to the next value. Only call with a valid value.
 */
void ds_value_next(DsValue* value);

/**
 * Is the value valid. A value is invalid if it is at the end of it's given
 * container e.g collection, row, result set. Or if an invalid index or name is
 * passed to `dse_row_column()` or `dse_row_column_by_name()`.
 */
bool ds_value_is_valid(const DsValue* value);

/**
 * Get the value type of the current value.
 */
DsValueType ds_value_type(const DsValue* value);

/**
 * Determine if a value is null.
 */
bool ds_value_is_null(const DsValue* value);

/**
 * Determine if a value is a collection.
 */
bool ds_value_is_collection(const DsValue* value);


/**
 * Get name of the current value, if applicable. Get's the column name for
 * column values and field name for UDT values. Returns NULL if not a column or
 * UDT field value.
 */
char* ds_value_name(const DsValue* value);

/**
 * Get the length of the current value's name, if applicable. Returns 0 if not a
 * column or UDT field value.
 */
size_t ds_value_name_length(const DsValue* value);

/**
 * Get UDT field for the supplied index. Returns an invalid value if the supplied
 * index is invalid.
 */
DsValue ds_value_field(const DsValue* value, size_t index);

/**
 * Get UDT field value for the supplied name. Returns an invalid value if
 * the supplied name is not valid for the UDT.
 */
DsValue ds_value_field_by_name(const DsValue* value, const char* name);

/**
 * Same as `ds_value_get_field()`
 */
DsValue ds_value_field_by_name_n(const DsValue* value, const char* name, size_t name_length);

/**
 * Get an integer from a value. It will return 0 if the value is not an integer or
 * max value if a value is too big for the current integer type. Use
 * `ds_value_type()` to call the correct function.
 */
int8_t ds_value_get_int8(const DsValue* value);
int16_t ds_value_get_int16(const DsValue* value);
int32_t ds_value_get_int32(const DsValue* value);
uint32_t ds_value_get_uint32(const DsValue* value);
int64_t ds_value_get_int64(const DsValue* value);

/**
 * Get an UUID form a value.
 */
DsUuid ds_value_get_uuid(const DsValue* value);

/**
 * Get the length of the current string value. It will return 0 if the value is not a string.
 */
size_t ds_value_get_string_length(const DsValue* value);

/**
 * Copy a string from the value and null-terminate. It returns the amount
 * copied into the string or 0 if the value is not a string.
 */
size_t ds_value_get_string(const DsValue* value, char* string, size_t string_length);

#endif // DATASTAX_HPP
