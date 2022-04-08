/*----------------------------------------------------------------------------------------------------------------------
                                          STUDENT NAME: YASMIN MUIN JWABREH
                                                 STUDENT ID: 1180815
                                                      PROJECT #3
                                                      BOOKSTORE
--------------------------------------------------------------------------------------------------------------------- */
/*                           This C BookStore Program Reads The Books Data From An Entered File Name.
 *                           It Stores The Books In An AVL Tree And Sort It Depending On Their Names
 *                           ( Book's Name ) Is The AVL Key. The User Can Search For A Specific Book
 *                           Insert A New One, And Delete. Also, The Books Data Are Listed On Their
 *                           Category (Fiction Or Non). The Program Also Counts The AVL Height.
 *                           Finally, It Allows The User To Save The Final BookStore Data On A
 *                           Specific File That He/She Want.
 *
 */
#include <stdio.h>
#include <afxres.h>

typedef struct Library *LibraryPtr;
struct Library
{
    char BookName[30];
    char AuthorName[30];
    int PubDate;
    char PubAddr[30];
    char Category[30];
    int height;
    LibraryPtr left;
    LibraryPtr right;
};
typedef struct Library BOOK;

LibraryPtr readFile();

LibraryPtr findBook(LibraryPtr tree, char name[], int z);

void inOrder(LibraryPtr, FILE *);

LibraryPtr insertBook(LibraryPtr, LibraryPtr *, char []);

int getHeight(LibraryPtr);


LibraryPtr findMin(LibraryPtr);

void FictionBooks(LibraryPtr tree);

void Non_FictionBooks(LibraryPtr tree);

LibraryPtr SRL(LibraryPtr);

LibraryPtr SLR(LibraryPtr);

LibraryPtr DLR(LibraryPtr);

LibraryPtr DRL(LibraryPtr);

void MainMenu(LibraryPtr);

LibraryPtr editNode(LibraryPtr tree, char name[]);

int main()
{
    printf("\n\t----------------------- WELCOME TO MY BOOKSTORE PROGRAM ----------------------- \n");
    LibraryPtr tree;
    tree = readFile();
    if(tree == NULL)
        return 0;
    MainMenu(tree);
    return 0;

}

// ----------------------------------------------- Read Data From File ---------------------------------------------- //
/* This Function Reads The Books Data From A Specified File And Insert Every Book Data To The AVL Tree As A New Node */
LibraryPtr readFile()
{
    /* Open The File After Scan It's Name And Check The Existence */
    FILE *file;
    // printf("\t\tEnter The File Name You Want To Read The Books Data From (eg. Data.txt)\n");
    char filename[FILENAME_MAX + MAX_PATH + 5] = "books.txt";
    // gets(filename);
    file = fopen(filename, "r");

    /* If The File Was Not Found Keep Scanning It's Name Until The User Enter An Exist File Name */
    if (file == NULL)
    {
        printf("\t\t( %s ) FILE COULD NOT BE FOUND! \n", filename);
        //  printf("\t\tPlease Enter The File Name Again, Make Sure You Write It In The Shown Format\n");
        //   gets(filename);
        // file = fopen(filename, "r");
        return NULL;

    }
    /* Divide The Strings From The File By "|" And Save Every String As It Suits The Data Field In The AVL Tree Node */
    char line[80];
    char *p;
    LibraryPtr tree = NULL;
    while (!feof(file))
    {
        fgets(line, 80, file);
        LibraryPtr book = NULL;//do not malloc it because it may have already existed
        int count = 0;//indicates which part of sentence is read
        p = strtok(line, "|\n");
        if (p == NULL)
            continue;
        while (p != NULL)
        {
            switch (count)
            {
            case 0:
                tree = insertBook(tree, &book,
                                  p);//the added book is pointed to by "book" to continue adding information to it
                break;
            case 1:
                strcpy(book->AuthorName, p);
                break;
            case 2:
                book->PubDate = atoi(p);
                break;
            case 3:
                strcpy(book->PubAddr, p);
                break;
            case 4:
                strcpy(book->Category, p);
                break;
            }
            p = strtok(NULL, "|\n");
            ++count;
        }
    }
  //  printf("     _____________________________________________________________________________________\n\n");
    printf("\n \t\tYOU READ THE BOOKS DATA FROM %s FILE SUCCESSFULLY!\n\n", filename);
    return tree;
}

// ------------------------------------------ Insert Book To The AVL Tree ------------------------------------------- //

//this function inserts a book to the tree and balance the tree after insertion
LibraryPtr insertBook(LibraryPtr tree, LibraryPtr *book, char name[])
{
    if (tree == NULL)//the position to which the book will be added is found, add it
    {
        LibraryPtr temp = (LibraryPtr) malloc(sizeof(BOOK));
        if (temp == NULL)
        {
            printf("the program has to stop,not enough memory\n");
            exit(0);
        }
        strcpy(temp->BookName, name);
        temp->left = NULL;
        temp->right = NULL;
        temp->height = 0;
        *book = temp;
        return temp;
    }
    if (strcasecmp(name, tree->BookName) == 0)//if the book already exists just point to it by the pointer book
        *book = tree;
    //if the book name is alphabetically less than the recent checked node go and check its left (insert to the left sub_tree)
    else if (strcasecmp(name, tree->BookName) < 0)
    {
        tree->left = insertBook(tree->left, book, name);
        //after adding to the left sub_tree check if the tree is balanced if not handle that
        if (getHeight(tree->left) - getHeight(tree->right) >= 2)
        {
            if (strcasecmp(name, tree->left->BookName) <
                    0)//this means that the book was added to the left of the left of this node"tree"
                tree = SLR(tree);//single rotate left to right
            else
                tree = DLR(tree);//double rotate left to right
        }
    }
    //if the book name is alphabetically greater than the recent checked node go and check its right (insert to the right sub_tree)
    else
    {
        tree->right = insertBook(tree->right, book, name);
        //check if the tree is still balanced after adding to the right
        if (getHeight(tree->right) - getHeight(tree->left) >= 2)
        {
            if (strcasecmp(name, tree->right->BookName) >
                    0)//this means that the book was added to the right of the right of this node
                tree = SRL(tree);//single rotate right to left
            else
                tree = DRL(tree);//double rotate right to left
        }
    }
    //update the height of each passed on node (since it is a recursion function)
    tree->height = max(getHeight(tree->left), getHeight(tree->right)) + 1;

    return tree;
}

// --------------------------------------------- Height Of The AVL Tree --------------------------------------------- //

//this function returns the height of the sub_tree "book"
int getHeight(LibraryPtr book)
{
    if (book == NULL)//because height is zero if there is a node without children
        return -1;
    else
        return book->height;
}
// -------------------------------------------- Single Left Right Rotate -------------------------------------------- //

//this function returns the sub_tree k2 after doing single rotate from left to right on it
LibraryPtr SLR(LibraryPtr k2)
{
    LibraryPtr k1 = k2->left;
    k2->left = k1->right;
    k1->right = k2;
    //k2 before k1 since k1 height depends on it
    k2->height = max(getHeight(k2->left), getHeight(k2->right)) + 1;
    k1->height = max(getHeight(k1->left), getHeight(k1->right)) + 1;
    return k1;
}
// -------------------------------------------- Single Right Left Rotate -------------------------------------------- //

//this function returns the sub_tree k1 after doing single rotate from right to left on it
LibraryPtr SRL(LibraryPtr k1)
{
    LibraryPtr k2 = k1->right;
    k1->right = k2->left;
    k2->left = k1;
    //k1 before k2 since k2 height depends on it
    k1->height = max(getHeight(k1->left), getHeight(k1->right)) + 1;
    k2->height = max(getHeight(k2->left), getHeight(k2->right)) + 1;
    return k2;
}
// -------------------------------------------- Double Left Right Rotate -------------------------------------------- //

//this function returns the sub_tree k3 after doing double rotate from left to right on it
LibraryPtr DLR(LibraryPtr k3)
{
    //double rotate is two single rotates
    k3->left = SRL(k3->left);
    return SLR(k3);
}

// -------------------------------------------- Double Right Left Rotate -------------------------------------------- //

//this function returns the sub_tree k1 after doing double rotate from right to left on it
LibraryPtr DRL(LibraryPtr k1)
{
    k1->right = SLR(k1->right);
    return SRL(k1);
}

// ---------------------------------------- Prints The AVL In The Right Order --------------------------------------- //

//this function prints the tree in order (left -> root -> right)
void inOrder(LibraryPtr tree, FILE *file)
{

    if (tree != NULL)
    {
        inOrder(tree->left, file);
        if (file == NULL)
            printf("\t%-20s %-20s %-20d %-25s %-20s \n", tree->BookName, tree->AuthorName, tree->PubDate, tree->PubAddr,
                   tree->Category);
        else
            fprintf(file, "%s|%s|%d|%s|%s\n", tree->BookName, tree->AuthorName, tree->PubDate,
                    tree->PubAddr,
                    tree->Category);
        inOrder(tree->right, file);
    }
}

// -------------------------------------- Lists The Library Books By Categories ------------------------------------- //
// this function prints the tree in order after checking the node category field "List Node On Their Category"
void FictionBooks(LibraryPtr tree)
{
    if (tree != NULL)
    {

        FictionBooks(tree->left);
        if (strcasecmp(tree->Category, "fiction") == 0)
            printf("\t%-20s %-20s %-20d %-25s %-20s \n", tree->BookName, tree->AuthorName, tree->PubDate, tree->PubAddr,
                   tree->Category);

        FictionBooks(tree->right);
    }
}

void Non_FictionBooks(LibraryPtr tree)
{
    if (tree != NULL)
    {

        Non_FictionBooks(tree->left);
        if (strcasecmp(tree->Category, "fiction") != 0)
            printf("\t%-20s %-20s %-20d %-25s %-20s \n", tree->BookName, tree->AuthorName, tree->PubDate, tree->PubAddr,
                   tree->Category);

        Non_FictionBooks(tree->right);
    }
}

// -------------------------------------------- Delete Node From AVL Tree ------------------------------------------- //

//this function deletes a book from the tree and returns a balanced tree

LibraryPtr deleteNode(LibraryPtr tree, char name[])
{
    if (tree == NULL)//if the node is not found do not do anything
        return NULL;
    //to find the node to delete: ....
    //if the name to delete is less than the node that is being checked go and check the left sub_tree of this node(delete from left)
    else if (strcasecmp(name, tree->BookName) < 0)
    {
        tree->left = deleteNode(tree->left, name);
        //check if the tree is balanced after deleting and if not balance it
        if (getHeight(tree->right) - getHeight(tree->left) >= 2)
        {
            //if the height of the left of the right of the node is longer than the height of the right of the right then the problem needs double rotate
            if (getHeight(tree->right->left) > getHeight(tree->right->right))
                tree = DRL(tree);//double rotate from right to left
            else
                tree = SRL(tree);//single rotate from right to left
        }
    }
    //if the name to delete is greater than the node that is being checked go and check the right sub_tree of this node(delete from right)
    else if (strcasecmp(name, tree->BookName) > 0)
    {
        tree->right = deleteNode(tree->right, name);
        //check if the tree is balanced after deleting and if not balance it
        if (getHeight(tree->left) - getHeight(tree->right) >= 2)
        {
            //if the height of the right of the left of the node is longer than the height of the left of the left then the problem needs double rotate
            if (getHeight(tree->left->right) > getHeight(tree->left->left))
                tree = DLR(tree);//double rotate from left to right
            else
                tree = SLR(tree);//single rotate from left to right
        }
    }
    else  //if the node to be deleted is found
    {
        /*if it has two children then replace it with the minimum on the right sub_tree of it(the tree is still binary search)
         then delete that minimum from right to remove duplicity*/
        if (tree->left && tree->right)
        {
            LibraryPtr temp = findMin(tree->right);
            strcpy(tree->AuthorName, temp->AuthorName);
            strcpy(tree->Category, temp->Category);
            strcpy(tree->PubAddr, temp->PubAddr);
            tree->PubDate = temp->PubDate;
            strcpy(tree->BookName, temp->BookName);
            tree->right = deleteNode(tree->right, temp->BookName);
            //balance after delete
            if (getHeight(tree->left) - getHeight(tree->right) >= 2)
            {
                if (getHeight(tree->left->right) > getHeight(tree->left->left))
                    tree = DLR(tree);
                else
                    tree = SLR(tree);
            }
        }
        else  //if the node has at most one child then just replace it with its child and free the child node
        {
            LibraryPtr child = NULL;
            if (tree->left == NULL)
                child = tree->right;
            if (tree->right == NULL)
                child = tree->left;
            free(tree);
            /*if the tree originally had more than one child on one side so the tree is not balanced
              since this block is entered only when tree has no children on the other side*/
            if (child != NULL)
                child->height = 0;
            return child;//replace node by its child
        }
    }
    //update the height of each node that was accessed while deleting(recursion)
    tree->height = max(getHeight(tree->left), getHeight(tree->right)) + 1;
    return tree;
}

// --------------------------------------- Find The Minimum Value Of AVL Tree  -------------------------------------- //
// this function returns a pointer to the node with the minimum value
LibraryPtr findMin(LibraryPtr tree)
{
    if (tree == NULL)
        return NULL;
    else if (tree->left != NULL)
        return findMin(tree->left);
    else
        return tree;
}
// ---------------------------------------------- Find A Specific Book ---------------------------------------------- //

//this function returns a pointer to the book node which we search for
LibraryPtr findBook(LibraryPtr tree, char name[], int z)
{

    if (z == 1)
    {
        if (tree == NULL)
            return NULL;
        else if (strcasecmp(name, tree->AuthorName) == 0)
            return tree;
        else if (strcasecmp(name, tree->AuthorName) < 0)
            return findBook(tree->left, name, z);
        else
            return findBook(tree->right, name, z);
    }
    else if (z == 2)
    {
        if (tree == NULL)
            return NULL;
        else if (strcasecmp(name, tree->BookName) == 0)
            return tree;
        else if (strcasecmp(name, tree->BookName) < 0)
            return findBook(tree->left, name, z);
        else
            return findBook(tree->right, name, z);
    }
    else
        return NULL;
}

// ---------------------------------------------- Edit A Specific Book ---------------------------------------------- //
LibraryPtr editNode(LibraryPtr tree, char name[])
{
    char tempname[30];
    char tempauthor[30];
    int tempdate;
    char tempcountry[20];
    char tempcateg[20];
    LibraryPtr Z = NULL;
    LibraryPtr found = findBook(tree, name, 2);

    printf("\t\tPress 1. To Edit BOOK'S NAME\n");
    printf("\t\tPress 2. To Edit AUTHOR'S NAME\n");
    printf("\t\tPress 3. To Edit Publish Date\n");
    printf("\t\tPress 4. To Edit Publisher Address\n");
    printf("\t\tPress 5. To Edit CATEGORY\n");
    int d;
    scanf("%d", &d);

    switch (d)
    {
    case 1:
        printf("\t\tEnter The New Book's Name:\n");
        scanf(" ");
        gets(tempname);//read new book name
        insertBook(tree, &Z, tempname);
        strcpy(Z->BookName, tempname);
        strcpy(Z->AuthorName, found->AuthorName);
        Z->PubDate = found->PubDate;
        strcpy(Z->PubAddr, found->PubAddr);
        strcpy(Z->Category, found->Category);
        tree = deleteNode(tree, name);
        return tree;


    case 2: //to edit author name
        printf("\t\tEnter The New Author's Name:\n");
        scanf(" ");
        gets(tempauthor);
        strcpy(found->AuthorName, tempauthor);
        break;


    case 3://to edit publish date

        printf("\t\tEnter The New Publish Date:\n");
        scanf("%d", &tempdate);
        found->PubDate = tempdate;


        break;


    case 4://to edit country name

        printf("\t\tEnter The New Publisher Address:\n");
        scanf(" ");
        gets(tempcountry);//read new country name
        strcpy(found->PubAddr, tempcountry);
        break;


    case 5://to edit category

        printf("\t\tEnter The New Category: (fiction of non-fiction)\n");
        scanf(" ");
        gets(tempcateg);//read new category
        strcpy(found->Category, tempcateg);


        break;


    default:
        printf("\t\tOops! You Entered An Invalid Entry\n");
        break;

    }
    return tree;


}


// ---------------------------------------------------- Main Menu --------------------------------------------------- //

//this function prints to the user a menu of avl functionalists to choose from and call functions to execute them
void MainMenu(LibraryPtr tree)
{
    int choice;
    char name[30], Author[30], Addr[30], Category[10];
    char filename[MAX_PATH + FILENAME_MAX + 5];
    int z, k;
    LibraryPtr book = NULL;
    while (1)
    {
        printf("                             ->         MENU         <-                      \n");
        printf("\t\tPLEASE ENTER THE NUMBER OF THE OPTION YOU WANT TO RUN NEXT.         \n");
        printf("\t\t1. Display The Library Of The Books Sorted By Their Names.           \n");
        printf("\t\t2. Search For A Specific Book In The Library.                            \n");
        printf("\t\t3. Insert A New Book To The Library.                \n");
        printf("\t\t4. Delete Or Edit A Specific Book From The Library.                                  \n");
        printf("\t\t5. Display Books In The Library Listed By Their Category\n");
        printf("\t\t6. Print The Tree Height.                      \n");
        printf("\t\t7. Save The Data To A Specific File.                         \n");
        printf("\t\t8. Exit the program.                         \n");
        scanf(" ");
        choice = getchar() - 48;
        switch (choice)
        {
        case 1:
            printf("\t\tThe Library List Of The Books Sorted By Their Names:           \n\n");
            printf("%-20s %-20s %-20s %-25s %-20s\n\n", "\tBook's Name", "Book's Author", "Date Published",
                   "Publisher Address", "Category");
            inOrder(tree, NULL);
            printf("   ________________________________________________________________________________________________________________\n\n");

            break;
        case 2:
            printf("\t\tWhat Would You Like To Search by:\n");
            printf("\t\t1.Author Name\n");
            printf("\t\t2.Book Name\n");
            scanf("%d", &z);
            if (z == 1)
                printf("\t\tEnter The Author Name Of The Book You Want To Search For:\n ");
            else if (z == 2)
                printf("\t\tEnter The Name Of The Book You Want To Search For:\n ");
            else if (z < 1 || z > 2)
            {
                printf("\t\tOops! You Entered An Invalid Number");
                break;
            }
            scanf(" ");
            gets(name);
            book = findBook(tree, name, z);
            if (book == NULL)
            {
                printf("\t\tOops! The %s Book Doesn't Exist..\n", name);
                printf("   ________________________________________________________________________________________________________________\n\n");
            }
            else
            {
                printf("\t\tThe %s Book Was Found :) ..\n", name);
                printf("   ________________________________________________________________________________________________________________\n\n");
                printf("%-20s %-20s %-20s %-25s %-20s\n\n", "\tBook's Name", "Book's Author", "Date Published",
                       "Publisher Address", "Category");
                printf("\t%-20s %-20s %-20d %-25s %-20s\n",
                       book->BookName, book->AuthorName, book->PubDate, book->PubAddr, book->Category);
                printf("   ________________________________________________________________________________________________________________\n\n");


            }
            break;
        case 3:
            printf("\t\tEnter The Name Of The Book You Want To Insert In The Library\n ");
            scanf(" ");
            gets(name);
            tree = insertBook(tree, &book, name);
            printf("\t\tEnter The Author Name Of The %s Book: \n", name);
            gets(Author);
            strcpy(book->AuthorName, Author);
            printf("\t\tEnter The Date Year The %s Book Was Published: \n", name);
            scanf("%d", &(book->PubDate));
            printf("\t\tEnter The Publisher Address Of The Book: \n ");
            scanf(" ");
            gets(Addr);
            strcpy(book->PubAddr, Addr);
            printf("\t\tEnter The Category Of The Book ( Fiction Or Non-Fiction )\n ");
            scanf(" ");

            gets(Category);
            strcpy(book->Category, Category);
            printf("\t\tThe Book Was Inserted In The Library Successfully! \n");
            printf("   ________________________________________________________________________________________________________________\n\n");
            break;
        case 4:
            printf("\t\tEnter The Option You Want To Run\n");
            printf("\t\t1.Edit A Specific Book Data\n");
            printf("\t\t2.Delete A Specific Book \n");
            scanf("%d", &k);
            if (k == 1)
            {
                printf("\t\tEnter The Name Of The Book You Want To Edit From The Library\n ");
                scanf(" ");
                gets(name);
                book = findBook(tree, name, 2);
                if (book != NULL)
                {
                    //Editing The Node
                    tree = editNode(tree, name);
                    printf("\t\tThe %s Book Was Edited Successfully! \n", name);
                    printf("   ________________________________________________________________________________________________________________\n\n");

                }
                else
                {
                    printf("\t\tThe %s Book Doesn't Exist In The Library! .. \n", name);
                    printf("   ________________________________________________________________________________________________________________\n\n");
                }


            }
            else if (k == 2)
            {
                printf("\t\tEnter The Name Of The Book You Want To Delete From The Library\n ");
                scanf(" ");
                gets(name);
                book = findBook(tree, name, 2);
                if (book != NULL)
                {
                    //remove node from the tree
                    tree = deleteNode(tree, name);
                    printf("\t\tThe %s Book Was Deleted From The Library Successfully! \n", name);
                    printf("   ________________________________________________________________________________________________________________\n\n");

                }
                else
                {
                    printf("\t\tThe %s Book Doesn't Exist In The Library! .. \n", name);
                    printf("   ________________________________________________________________________________________________________________\n\n");
                }
            }
            break;
        case 5:
            printf("\t\tThe Library List Of The Books By Their Category:           \n\n");
            printf("                      ->         Fiction Books List         <-                      \n\n");
            printf("%-20s %-20s %-20s %-25s %-20s\n\n", "\tBook's Name", "Book's Author", "Date Published",
                   "Publisher Address", "Category");
            FictionBooks(tree);
            printf("\n");
            printf("   ________________________________________________________________________________________________________________\n\n");

            printf("                      ->         Non-Fiction Books List         <-                      \n\n");
            printf("%-20s %-20s %-20s %-25s %-20s\n\n", "\tBook's Name", "Book's Author", "Date Published",
                   "Publisher Address", "Category");

            Non_FictionBooks(tree);
            printf("   ________________________________________________________________________________________________________________\n\n");
            break;
        case 6:
            printf("\t\tThe Tree Height Is %d\n", tree->height);
            printf("   _________________________________________________________________________\n\n");
            break;
        case 7:
            printf("\t\tEnter The Name Of The File To Print In: (eg: books.txt)\n ");
            scanf(" ");
            gets(filename);
            FILE *file;
            file = fopen(filename, "w");
            // fprintf(file, "%s|%s|%s|%s|%s\n", "Book's Name", "Book's Author", "Date Published","Publisher Address", "Category");


            inOrder(tree, file);
            printf(" \t\tYOU SAVED THE DATA IN %s FILE SUCCESSFULLY! \n ", filename);
            printf("   _________________________________________________________________________\n\n");

            fclose(file);
            break;
        case 8:
            printf("\t\tExiting From The Program.. \n");
            printf("   ________________________________________________________________________\n\n");
            exit(1);
        default:
            printf("\t\tYou Entered An Invalid Number!.. \n");
            printf("   _________________________________________________________________________\n\n");


        }
    }
}
