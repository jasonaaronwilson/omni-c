#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef id_t = uint32_t;

typedef data_item_t = struct {
  id_t id;
  uint32_t value;
  data_item_t* next;
};

data_item_t create_data_item(id_t id, uint32_t value) {
  data_item_t item = {0};
  item.id = id;
  item.value = value;
  return item;
}

int main() {
  data_item_t item_1 = create_data_item(12345, 100);
  data_item_t item_2 = create_data_item(67890, 200);
  item_1.next = &item_2;
  if (item_1.next->id != 67890) {
    return 1;
  }
  return 0;
}
