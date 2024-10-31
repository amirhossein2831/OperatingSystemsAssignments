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
int server_fd;
int client_socket;

// Define a struct to hold data
typedef struct {
    unsigned int id;
    char name[50];
    int amount;
} Product;

// 0 for success,-1 for failed allocate,-2 for duplicated name
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

// if the result is -1 product not found,0 for success
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

// if the result is -1 product not found,0 for success
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

// if the result is -1 product not found,0 for success
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

char* sendProductsList() {
    char product[100];
    char *response = malloc(10000 * sizeof(char));
    if (response == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }
    response[0] = '\0';

    if (list == NULL) {
        response = "You have no Product :)";
    }

    for (GList *l = list; l != NULL; l = l->next) {
        Product *p = (Product *)l->data;
        snprintf(product, sizeof(product), "ID: %d, Name: %s, Amount: %d|", p->id, p->name, p->amount);
        strcat(response, product);
    }

    return response;
}

void freeList() {
    for (GList *l = list; l != NULL;) {
        GList *next = l->next;
        Product *p = (Product *) l->data;
        free(p);
        g_list_free_1(l);
        l = next;
    }
}

// Signal handler for cleanup
void handle_shutdown(int signum) {
    printf("\nShutting down server gracefully...\n");
    close(client_socket);
    close(server_fd);
    exit(0);
}

void create_server() {
    signal(SIGINT, handle_shutdown);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    const int address_len = sizeof(address);

    if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Listening on %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

    if ((client_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &address_len)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    printf("Client connected: %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

}

int main() {
    create_server();
    char buffer[1024] = {0};

    while (1) {
        char *tokens[10];
        char *response = "";
        int valread = read(client_socket, buffer, 1024);
        if (valread <= 0) {
            break;
        }

        char *token = strtok(buffer, "|");
        for (int i = 0;token != NULL && i < 10;i++) {
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
            if (res == -1) {
                response = "Fail to allocate memory";
            } else if (res == -2) {
                response = "The name must be unique";
            } else {
                response = "Product created successfully";
            }
        }
            // Increase Amount Command
        else if (strcmp(tokens[0], "Add") == 0) {
            int res = increaseProductAmount(tokens[1], atoi(tokens[2]));
             if (res == -1) {
                response = "There is no item with given name";
            }else {
                response = "amount of product increase successfully";
            }
        }
            // Reduce Amount Command
        else if (strcmp(tokens[0], "Reduce") == 0) {
            int res = reduceProductAmount(tokens[1], atoi(tokens[2]));
            if (res == -1) {
                response = "There is no item with given name";
            }else {
                response = "amount of product reduce successfully";
            }
        }
            // Remove Product Command
        else if (strcmp(tokens[0], "Remove") == 0) {
            int res = removeProduct(tokens[1]);
            if (res == -1) {
                response = "There is no item with given name";
            }else {
                response = "product removed successfully";
            }
        }

        // Send response
        send(client_socket, response, strlen(response), 0);

        // Clear the buffer
        memset(buffer, 0, sizeof(buffer));
    }

    handle_shutdown(0);
    freeList();
    return 0;
}
