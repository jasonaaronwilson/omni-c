typedef int value_array_t;

struct declarations_node_S {
    node_list_t declarations;
};

typedef struct node_list_S node_list_t;

struct node_list_S {
    value_array_t* list;
};
