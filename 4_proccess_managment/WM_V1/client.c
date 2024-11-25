#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>


#define PORT 8080

int socket_connect() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\nSocket creation error\n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported\n");
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed\n");
        return -1;
    }

    return sock;
}

int main() {
    int valread;
    char buffer[2048] = {0};
    char response[100];
    char request[1000];

    int sock = socket_connect();

    printf("For showing the list of products, enter: List \n");
    printf("For creating a product, enter: Create \n");
    printf("For Increase Amount of a product, enter: Add \n");
    printf("For Reduce Amount of a product, enter: Reduce \n");
    printf("For Remove a product, enter: Remove \n");
    printf("For exit, enter: Exit \n");

    while (1) {
        scanf("%s", response);

        // List Command
        if (strcmp(response, "List") == 0) {
            snprintf(request, sizeof(request), "%s", response);
        }
            // Create Command
        else if (strcmp(response, "Create") == 0) {
            char product_name[100];
            int amount;

            printf("Enter product name: ");
            scanf("%99s", product_name);

            getchar();

            printf("Enter product amount (press Enter for default 0): ");
            char amount_input[10];
            fgets(amount_input, sizeof(amount_input), stdin);
            if (strlen(amount_input) == 0 || amount_input[0] == '\n') {
                amount = 0; // Default to 0 if no input is given
            } else {
                sscanf(amount_input, "%d", &amount);
            }

            snprintf(request, sizeof(request), "%s|%s|%d", response, product_name, amount);
        }
            // Increase Amount Command
        else if (strcmp(response, "Add") == 0) {
            char product_name[100];
            int amount;

            printf("Enter product name: ");
            scanf("%99s", product_name);

            getchar();

            printf("Enter the amount to increase: ");
            scanf("%d", &amount);

            snprintf(request, sizeof(request), "%s|%s|%d", response, product_name, amount);
        }
            // Reduce Amount Command
        else if (strcmp(response, "Reduce") == 0) {
            char product_name[100];
            int amount;

            printf("Enter product name: ");
            scanf("%99s", product_name);

            getchar();

            printf("Enter the amount to increase: ");
            scanf("%d", &amount);

            snprintf(request, sizeof(request), "%s|%s|%d", response, product_name, amount);
        }
            // Remove Product Command
        else if (strcmp(response, "Remove") == 0) {
            char product_name[100];

            printf("Enter product name: ");
            scanf("%99s", product_name);

            snprintf(request, sizeof(request), "%s|%s", response, product_name);
        }
            // Exit Command
        else if (strcmp(response, "Exit") == 0) {
            break;
        }
            // Unknown Command
        else {
            printf("Unknown command. Please enter List, Create, or Exit.\n");
            continue; // Skip sending unknown commands
        }

        // Send message
        send(sock, request, strlen(request), 0);

        // Get response
        valread = read(sock, buffer, sizeof(buffer) - 1);
        if (valread < 0) {
            break;
        }
        buffer[valread] = '\0';

        printf("******* Response from the server *******\n");
        char *token = strtok(buffer, "|");
        while (token != NULL) {
            printf("Token: %s\n", token);
            token = strtok(NULL, "|");
        }
        printf("*******      End of Response     *******\n");

        memset(buffer, 0, sizeof(buffer));
    }

    close(sock);
    return 0;
}
