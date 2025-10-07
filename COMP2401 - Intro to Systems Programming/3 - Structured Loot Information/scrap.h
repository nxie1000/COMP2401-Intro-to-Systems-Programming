#define MAX_STR 64
#define MAX_SCRAP 16

#define ERR_OK 0
#define ERR_MAX_CAPACITY -1
#define ERR_NULL_POINTER -2
#define ERR_NOT_IMPLEMENTED -3

#define CONDITION_USELESS 0
#define CONDITION_POOR 1
#define CONDITION_TYPICAL 3
#define CONDITION_PRISTINE 6
#define MAX_CONDITION 6

struct ScrapType {
    char name[MAX_STR];
    float base_value;
    int discovered_count;
};

struct Scrap {
    struct ScrapType* type;
    int condition; // One of CONDITION_ definitions. For those curious, an enum type might be more useful here.
};

struct ScrapCollection {
    int size;
    struct Scrap elements[MAX_SCRAP];
};

int init_scrap_type(struct ScrapType* type, const char* name, float base_value);
int init_scrap(struct Scrap* scrap, struct ScrapType* type, int condition);
int init_scrap_collection(struct ScrapCollection* collection);

int compare_scrap(const struct Scrap* a, const struct Scrap* b);
int add_scrap(struct ScrapCollection* collection, struct Scrap scrap);

int get_value(const struct Scrap* scrap, float* value);
int print_scrap(const struct Scrap* scrap);
int print_scrap_collection(const struct ScrapCollection* collection);

