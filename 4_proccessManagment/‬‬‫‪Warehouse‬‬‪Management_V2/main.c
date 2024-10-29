#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

GList *list = NULL;
int PORT = 8080;
int SERVER_FD;
int CLIENT_SOCKET;
volatile sig_atomic_t RUNNING = 1;

// Define a struct to hold data
typedef struct {
    unsigned int id;
    char name[50];
    int amount;
} Product;


// List and Product Functions
int addProduct(const char *, int);
int increaseProductAmount(char *, int );
int reduceProductAmount(char *, int );
int removeProduct(const char *);
char *sendProductsList();
void freeList();

// Server Functions
void handle_shutdown(int);
void setup_server();
void handle_client(struct sockaddr_in);
int process_command(char *);


// AddProduct to User Product: 0 for success,-1 for failed allocate,-2 for duplicated name
int addProduct(const char *name, int amount) {
    unsigned int currentLength = g_list_length(list);

    for (GList *l = list; l != NULL; l = l->next) {
        Product *p = (Product *) l->data;

        if (strcmp(p->name, name) == 0) {
            return -2;
        }
    }

    Product *p = malloc(sizeof(Product));
    if (p == NULL) {
        return -1;
    }

    p->id = currentLength + 1;
    snprintf(p->name, sizeof(p->name), "%s", name);
    p->amount = amount;

    list = g_list_append(list, p);
    return 0;
}

// Increase the Amount of a Product: the result is -1 product not found,0 for success
int increaseProductAmount(char *name, int change) {
    for (GList *l = list; l != NULL; l = l->next) {
        Product *p = (Product *) l->data;

        if (strcmp(p->name, name) == 0) {
            p->amount += change;
            return 0;
        }
    }
    return -1;
}

// Reduce the Amount of a Product: if the result is -1 product not found,0 for success
int reduceProductAmount(char *name, int change) {
    for (GList *l = list; l != NULL; l = l->next) {
        Product *p = (Product *) l->data;

        if (strcmp(p->name, name) == 0) {
            p->amount -= change;
            return 0;
        }
    }
    return -1;
}

// Remove a Product: if the result is -1 product not found,0 for success
int removeProduct(const char *name) {
    GList *current = list;
    GList *prev = NULL;

    while (current != NULL) {
        Product *p = (Product *) current->data;

        if (strcmp(p->name, name) == 0) {
            if (prev == NULL) {
                list = g_list_remove_link(list, current);
            } else {
                prev->next = current->next;
                g_list_free_1(current);
            }

            free(p);
            return 0;
        }

        prev = current;
        current = current->next;
    }

    return -1;
}

// Prepare a response for product list
char *sendProductsList() {
    char product[100];
    char *response = malloc(10000 * sizeof(char));
    if (response == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }
    response[0] = '\0';

    if (list == NULL) {
        response = "You have no Product :)";
        return response;
    }

    for (GList *l = list; l != NULL; l = l->next) {
        Product *p = (Product *) l->data;
        snprintf(product, sizeof(product), "ID: %d, Name: %s, Amount: %d|", p->id, p->name, p->amount);
        strcat(response, product);
    }

    return response;
}

// free up the list
void freeList() {
    for (GList *l = list; l != NULL;) {
        GList *next = l->next;
        Product *p = (Product *) l->data;
        free(p);
        g_list_free_1(l);
        l = next;
    }
}

// Handler for shutdown when a signal come
void handle_shutdown(int signum) {
    static volatile sig_atomic_t shutting_down = 0;

    if (shutting_down) {
        return;
    }

    shutting_down = 1; // Mark as shutting down
    RUNNING = 0; // Update the running flag
    printf("Server shutting down...\n");

    close(SERVER_FD); // Close the server socket
    printf("Free the memory...\n");
    freeList(); // Free the product list
}

// Setup server socket
void setup_server() {
    struct sockaddr_in address;

    // Create server socket
    if ((SERVER_FD = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Initialize address struct
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket to the address
    if (bind(SERVER_FD, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening on the server socket
    if (listen(SERVER_FD, 10) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Listening on %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
}

// Handle a client connection
void handle_client(struct sockaddr_in address) {
    char buffer[1024] = {0};
    char client_info[100];

    snprintf(client_info, sizeof(client_info), "%s:%d", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
    while (1) {
        int valread = read(CLIENT_SOCKET, buffer, 1024);
        if (valread <= 0) {
            printf("Client %s disconnected.\n", client_info);
            break;
        }

        int res = process_command(buffer);
        if (res == -1) {
            printf("Client %s disconnected.\n", client_info);            break;
        }
        memset(buffer, 0, sizeof(buffer));
    }

    close(CLIENT_SOCKET);
}

// Process a command from the client
int process_command(char *buffer) {
    char *tokens[10] = {0};
    char *response = "";

    // Tokenize the input command
    char *token = strtok(buffer, "|");
    for (int i = 0; token != NULL && i < 10; i++) {
        tokens[i] = token;
        token = strtok(NULL, "|");
    }

    // List Command
    if (strcmp(tokens[0], "List") == 0) {
        response = sendProductsList();
    }
        // Create Command
    else if (strcmp(tokens[0], "Create") == 0) {
        int res = addProduct(tokens[1], atoi(tokens[2]));
        response = (res == -1) ? "Fail to allocate memory" :
                   (res == -2) ? "The name must be unique" : "Product created successfully";
    }
        // Increase Amount Command
    else if (strcmp(tokens[0], "Add") == 0) {
        int res = increaseProductAmount(tokens[1], atoi(tokens[2]));
        response = (res == -1) ? "There is no item with given name" : "Amount of product increased successfully";
    }
        // Reduce Amount Command
    else if (strcmp(tokens[0], "Reduce") == 0) {
        int res = reduceProductAmount(tokens[1], atoi(tokens[2]));
        response = (res == -1) ? "There is no item with given name" : "Amount of product reduced successfully";
    }
        // Remove Product Command
    else if (strcmp(tokens[0], "Remove") == 0) {
        int res = removeProduct(tokens[1]);
        response = (res == -1) ? "There is no item with given name" : "Product removed successfully";
    }
    else if (strcmp(tokens[0], "Exit") == 0){
        return -1;
    }

    // Send response
    send(CLIENT_SOCKET, response, strlen(response), 0);
    return 0;
}

// Start server
void start_server() {
    struct sockaddr_in address;
    const int address_len = sizeof(address);

    while (RUNNING) {
        if ((CLIENT_SOCKET = accept(SERVER_FD, (struct sockaddr *)&address, (socklen_t *)&address_len)) < 0) {
            if (!RUNNING) break;
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        printf("Client connected: %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

        if (fork() == 0) {
            close(SERVER_FD);
            handle_client(address);
            exit(0);
        }

        close(CLIENT_SOCKET);
    }
}

// Main function
int main() {
    // handle kill signal for shutdown
    signal(SIGINT, handle_shutdown);
    signal(SIGTERM, handle_shutdown);

    // setup server
    setup_server();

    // start server
    start_server();

    return 0;
}
