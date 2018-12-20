#include "datastax.h"
#include <stdio.h>

int main() {
  DsResultSet* rs;

  // Retrieve result set

  { // Print first row
    DsRow row = ds_row_init(rs);
    if (ds_row_is_valid(&row)) {
      DsValue release_version = ds_row_column_by_name(&row, "release_version"); // text

      char release_version_string[256];
      ds_value_get_string(&release_version,
                          release_version_string,
                          sizeof(release_version_string));
      printf("release_version: %s\n", release_version_string);
    }
  }

  { // Print entire result set
    for (DsRow row = ds_row_init(rs); ds_row_is_valid(&row); ds_row_next(&row)) {
      DsValue key = ds_row_column_by_name(&row, "key"); // text
      DsValue value = ds_row_column_by_name(&row, "value"); // uuid

      char key_string[256];
      char value_string[DS_UUID_STRING_LENGTH];
      ds_value_get_string(&key, key_string, sizeof(key_string));
      ds_uuid_string(ds_value_get_uuid(&value), value_string);
      printf("key: %s, value %s\n", key_string, value_string);
    }
  }

  { // Print entire result set by column
    for (DsValue value = ds_value_init(rs); ds_value_is_valid(&value); ds_value_next(&value)) {
      char string[256];

      printf("column name: %s, column_value ", ds_value_name(&value));
      switch(ds_value_type(&value)) {
        case DS_VALUE_TYPE_INT:
          printf("%d\n", ds_value_get_int32(&value));
          break;
        case DS_VALUE_TYPE_VARCHAR:
        case DS_VALUE_TYPE_TEXT:
          ds_value_get_string(&value, string, sizeof(string));
          printf("%s\n", string);
          break;
        case DS_VALUE_TYPE_UUID:
            ds_uuid_string(ds_value_get_uuid(&value), string);
            printf("%s\n", string);
          break;
        default:
          printf("<unhandled>\n");
          break;
      }
    }
  }

  { // Print set, but this would work for a list, map, or tuple too
    DsRow row = ds_row_init(rs);
    if (ds_row_is_valid(&row)) {
      DsValue phone_numbers = ds_row_column_by_name(&row, "phone_numbers"); // set<int>

      printf("phone_numbers: ");
      for (DsValue phone_number = ds_value_init_in(&phone_numbers);
           ds_value_is_valid(&phone_number);
           ds_value_next(&phone_number)) {
        printf("%d ", ds_value_get_int32(&phone_number));
      }
      printf("\n");
    }
  }

  { // Print UDT fields
    DsRow row = ds_row_init(rs);
    if (ds_row_is_valid(&row)) {
      // CREATE TYPE cycling.basic_info ( birthday timestamp, nationality text, weight text, height text);
      DsValue basic_info = ds_row_column_by_name(&row, "info"); //

      printf("Basic info:\n");
      for (DsValue field = ds_value_init_in(&basic_info);
           ds_value_is_valid(&field);
           ds_value_next(&field)) {
        char string[256];
        printf("%s: ", ds_value_name(&field));
        switch (ds_value_type(&field)) {
          case DS_VALUE_TYPE_TIMESTAMP:
            printf("%ld\n", ds_value_get_int64(&field));
            break;
          case DS_VALUE_TYPE_TEXT:
            ds_value_get_string(&field, string, sizeof(string));
            printf("%s\n", string);
            break;
        }
      }
    }
  }

  { // Print UDT specific UDT value
    DsRow row = ds_row_init(rs);
    if (ds_row_is_valid(&row)) {
      // CREATE TYPE cycling.basic_info ( birthday timestamp, nationality text, weight text, height text);
      DsValue basic_info = ds_row_column_by_name(&row, "info"); //

      DsValue birthday = ds_value_field_by_name(&basic_info, "birthday");
      printf("%s: %ld\n", ds_value_name(&birthday), ds_value_get_int64(&birthday));
    }
  }
}
