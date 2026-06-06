#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX_LENGTH 100

enum statuses {incomplete, completed};
enum tags {study, home, personal};

typedef struct node * tree;
typedef struct node * task;

const char* statusArr[] = {"incomplete", "completed"};
const char* tagArr[] = {"study", "home", "personal"};

struct node{
    char title[MAX_LENGTH];
    int startDate;
    int dueDate;
    int time;
    enum statuses status;
    enum tags tag;
    struct node* left;
    struct node* right;
};

struct node* makeNode(char title[] ,int sDate, int dDate, int t, enum statuses sta, enum tags tg){
    task p = (struct node*)malloc(sizeof(struct node));
    strcpy(p->title, title);
    p->startDate = sDate;
    p->dueDate = dDate;
    p->time = t;
    p->status = sta;
    p->tag = tg;
    p->left = NULL;
    p->right = NULL;
    return p;
}

//insert p to T
struct node* insert(tree T, task p){
    if(T == NULL) return p;
    if(p->dueDate > T->dueDate){
        T->right = insert(T->right, p);
    }
    else{ //p->dueDate <= T->dueDate -> node moi day sang ben trai
        T->left = insert(T->left, p);
    }
    return T;
}

// lưu vào file
void saveToFile(tree T, FILE *f) {
    if (T == NULL) return;
    // ghi dữ liệu node hiện tại vào file, phân cách bằng |
    fprintf(f, "%s|%d|%d|%d|%d|%d\n", T->title, T->startDate, T->dueDate, T->time, T->status, T->tag);
    saveToFile(T->left, f);
    saveToFile(T->right, f);
}

// lưu cây
void saveTree(tree T) {
    FILE *f = fopen("tasks.txt", "w");
    if (f == NULL) return;
    saveToFile(T, f);
    fclose(f);
}

tree loadFromFile() {
    FILE *f = fopen("tasks.txt", "r");
    if (f == NULL) return NULL;

    tree T = NULL;
    char title[MAX_LENGTH];
    int sD, dD, t, sta, tg;

    // Đọc từng dòng cho đến hết file
    while (fscanf(f, " %99[^|]|%d|%d|%d|%d|%d\n", title, &sD, &dD, &t, &sta, &tg) != EOF) {
        task p = makeNode(title, sD, dD, t, (enum statuses)sta, (enum tags)tg);
        T = insert(T, p);
    }
    fclose(f);
    return T;
}

//traverse
int count;
void traverse(tree T){
    if (T == NULL) return;
    traverse(T->left);
    printf("%2d | %-20s | %-10d | %-10d | %-4d | %-12s | %-10s\n", count++, T->title, T->startDate, T->dueDate, T->time, statusArr[T->status], tagArr[T->tag]);
    traverse(T->right);
}

//copy value
void copyNode(task p, task q){ //p = q
    if(p == NULL || q == NULL) return;
    strcpy(p->title, q->title);
    p->startDate = q->startDate;
    p->dueDate = q->dueDate;
    p->time = q->time;
    p->status = q->status;
    p->tag = q->tag;

}

//delete 1 node
void delNode(task *P){
    task Q, R;

    R = *P;
    if((*P)->left == NULL){ //P has only right child or no child
        Q = *P;
        *P = (*P)->right;
        free(Q);
    }
    else {
        if((*P)->right == NULL){ //P has only left child
            Q = *P;
            *P = (*P)->left;
            free(Q);
        }
        else{ //P is a double node -> P = max left
            Q = (*P)->left; 
            if(Q->right == NULL){
                copyNode(*P, Q);
                (*P)->left = Q->left;
            }
            else{
                while(Q->right != NULL){
                    R = Q; //R: parent of max left (Q)
                    Q = Q->right;
                }
                copyNode(*P, Q);
                R->right = Q->left;
            }
            free(Q);
        }
    }
}

//delete same due date
int deleted;
void delSameDue(tree *T, int dueDate, char title[]){
    if (*T == NULL) return;
    if (dueDate < (*T)->dueDate) delSameDue(&(*T)->left, dueDate, title);
    else if (dueDate > (*T)->dueDate) delSameDue(&(*T)->right, dueDate, title);
    else {
        if (strcmp((*T)->title, title) == 0){
            delNode(T);
            deleted = 1;
        }
        else delSameDue(&(*T)->left, dueDate, title); //node bằng nhau luôn bên trái (do đặt vậy trong insert)
    }
}

//print node
void printTask(task p){
    if (p == NULL) return;
    printf("   | %-20s | %-10d | %-10d | %-4d | %-12s | %-10s\n", 
         p->title, p->startDate, p->dueDate, p->time, statusArr[p->status], tagArr[p->tag]);
}

//search by due date
int found;
void search(tree T, int dueDate) {
    if (T == NULL) return;

    if (dueDate < T->dueDate) {
        search(T->left, dueDate);
    }
    else if (dueDate > T->dueDate) {
        search(T->right, dueDate);
    }
    else { // dueDate == T->dueDate
        found = 1;
        printTask(T);
        search(T->left, dueDate);
    }
}

//filter by tag
void findTag(tree T, enum tags tg){
    if (T == NULL) return;

    findTag(T->left, tg);
    if (T->tag == tg) {
        printTask(T);
    }
    findTag(T->right, tg);
}


//main function
void showList(tree T){
    count = 1;
    printf("%2d | %-20s | %-10s | %-10s | %-4s | %-12s | %-10s\n", 0, "title", "start date", "due date", "time", "status", "tag");
    traverse(T);
}

tree addTask(tree T){
    char title[MAX_LENGTH];
    int staD, dueD, t, stat, tg;
    printf("title: \n");
    scanf(" %99[^\n]", title);

    printf("start date: \n");
    if (scanf("%d", &staD) != 1) { //ham scanf return so bien gan gia tri thanh cong
        printf("Invalid start date\n");
        while (getchar() != '\n'); //doc va bo ky tu thua
        return T;
    }

    printf("due date: \n");
    if (scanf("%d", &dueD) != 1) {
        printf("Invalid due date\n");
        while (getchar() != '\n');
        return T;
    }

    printf("time (hour): ");
    if (scanf("%d", &t) != 1) {
        printf("Invalid time\n");
        while (getchar() != '\n');
        return T;
    }

    printf("status (0: incomplete, 1: completed): ");
    if (scanf("%d", &stat) != 1 || stat < 0 || stat > 1) {
        printf("Invalid status\n");
        while (getchar() != '\n');
        return T;
    }

    printf("tag (0: study, 1: home, 2: personal): ");
    if (scanf("%d", &tg) != 1 || tg < 0 || tg > 2) {
        printf("Invalid tag\n");
        while (getchar() != '\n');
        return T;
    }

    task p = makeNode(title, staD, dueD, t, stat, tg);
    T = insert(T, p);

    saveTree(T);
    printf("Task created successfully\n");

    return T;
}

void searchByDueDate(tree T){
    int dueDate;
    printf("Enter due date: ");
    if (scanf("%d", &dueDate) != 1){
        printf("Invalid due date!\n");
        while (getchar() != '\n'); 
        return;
    }
    
    printf("\nTasks with due date = %d\n", dueDate);
    printf("   | %-20s | %-10s | %-10s | %-4s | %-12s | %-10s\n", "title", "start date", "due date", "time", "status", "tag");

    found = 0;
    search(T, dueDate);

    if (found == 0){
        printf("No task found with this due date\n");
    }
}

void deleteTask(tree *T){
    int dueDate;
    char title[MAX_LENGTH];

    printf("Enter due date of task to delete: ");
    if (scanf("%d", &dueDate) != 1){
        printf("Invalid due date!\n");
        while (getchar() != '\n');
        return;
    }

    printf("Enter title of task to delete: ");
    scanf(" %99[^\n]", title);

    deleted = 0;
    delSameDue(T, dueDate, title);
    if (deleted){
        saveTree(*T);
        printf("Task deleted successfully\n");
    }
    else printf("Task not found\n");
}

void filterByTag(tree T){
    int tg;

    printf("Enter tag (0: study, 1: home, 2: personal): ");
    if (scanf("%d", &tg) != 1 || tg < 0 || tg > 2){
        printf("Invalid tag\n");
        while (getchar() != '\n');
        return;
    }

    printf("\nTasks with tag = %s\n", tagArr[tg]);
    printf("   | %-20s | %-10s | %-10s | %-4s | %-12s | %-10s\n", "title", "start date", "due date", "time", "status", "tag");
    findTag(T, (enum tags)tg);
}

//menu
void printMenu(){
    printf("\n========== TASK MANAGER ==========\n");
    printf("1. Show all tasks\n");
    printf("2. Add new task\n");
    printf("3. Search tasks by due date\n");
    printf("4. Delete a task (by due date + title)\n");
    printf("5. Filter tasks by tag\n");
    printf("0. Exit\n");
    printf("Choose an option: ");
}

void menu(tree *T){
    int choice;
    while (1) {
        printMenu();
        scanf("%d", &choice);

        if (choice == 0) {
            printf("Exiting program...\n");
            break;
        }
        else if (choice == 1) {
            showList(*T);
        }
        else if (choice == 2) {
            *T = addTask(*T);
        }
        else if (choice == 3) {
            searchByDueDate(*T);
        }
        else if (choice == 4) {
            deleted = 0;
            deleteTask(T);
        }
        else if (choice == 5) {
            filterByTag(*T);
        }
        else {
            printf("Invalid choice. Try again.\n");
        }
    }
}

int main(){

    tree T = loadFromFile();
    showList(T);
    menu(&T);
    
    return 0;
}