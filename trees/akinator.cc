#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct Ttree
{
  char *text;
  struct Ttree *left, *right;
};

Ttree *tree = nullptr, *p = nullptr;
char *temp;
char *correct_answer;
char *difference;

Ttree *loadNode(FILE *file)
{
  char *buffer = (char *)malloc(32 * sizeof(char));
  if (fscanf(file, "%s", buffer) == 1)
  {
    if (strcmp(buffer, "NULL") == 0)
    {
      return NULL;
    }
    else
    {
      Ttree *node = (Ttree *)malloc(sizeof(Ttree));
      node->text = (char *)malloc(32 * sizeof(char));
      strcpy(node->text, buffer);
      node->left = loadNode(file);
      node->right = loadNode(file);
      return node;
    }
  }
  else
  {
    return NULL;
  }
}

Ttree *loadTree(const char *filename)
{
  FILE *file = fopen(filename, "r");
  if (file == nullptr)
  {
    return NULL;
  }

  Ttree *tree = loadNode(file);

  fclose(file);
  return tree;
}

Ttree *create_tree(const char *filename)
{
  Ttree *tree = loadTree(filename);
  if (tree == NULL)
  {

    tree = (Ttree *)malloc(sizeof(Ttree));
    tree->text = (char *)malloc(32 * sizeof(char));
    strcpy(tree->text, "Animal");
  }
  return tree;
}

bool isEmpty(Ttree *tree)
{
  return tree == NULL;
}

void paintLeaf(Ttree *tree, int BaseSpace, int depth)
{
  Ttree *point = tree;
  if (point->right != NULL)
    paintLeaf(point->right, BaseSpace, depth + 1);

  for (int i = 1; i < BaseSpace + depth * 3; i++)
    printf(" ");
  printf("%s\n", point->text);

  if (point->left != NULL)
    paintLeaf(point->left, BaseSpace, ++depth);
}

void ShowTree(Ttree *tree, int BaseSpace)
{
  paintLeaf(tree, BaseSpace, 0);
}

void updateTree(Ttree *current, char *new_answer, char *difference)
{

  current->left = (Ttree *)malloc(sizeof(Ttree));
  current->right = (Ttree *)malloc(sizeof(Ttree));

  current->left->text = (char *)malloc(32 * sizeof(char));
  current->right->text = (char *)malloc(32 * sizeof(char));

  temp = current->text;

  current->text = difference;

  current->left->text = temp;
  current->right->text = new_answer;

  current->left->left = nullptr;
  current->left->right = nullptr;

  current->right->left = nullptr;
  current->right->right = nullptr;

  // printf("\nCurrent:%s, Correct: %s, Difference: %s \n", current->text, current->left->text, current->right->text);
}

void addNewAnswer(Ttree *current)
{

  printf("Que era? ");
  scanf("%s", correct_answer);

  printf("Diferencia entre %s y %s? ", current->text, correct_answer);
  scanf("%s", difference);

  // printf("\nCurrent:%s, Correct: %s, Difference: %s \n", current->text, correct_answer, difference);

  updateTree(current, correct_answer, difference);

  printf("Ok\n");
}

void guessObject(Ttree *tree)
{
  Ttree *current = tree;
  char answer;

  while (current->left != nullptr || current->right != nullptr)
  {
    printf("%s? (y/n): ", current->text);

    scanf(" %c", &answer);

    if (answer == 'y' || answer == 'Y' || answer == 'S' || answer == 's')
    {
      if (current->left != nullptr)
      {
        current = current->left;
      }
    }
    else
    {
      if (current->right != nullptr)
      {
        current = current->right;
      }
    }
  }

  printf("Ya lo se! Es %s?\n", current->text);
  scanf(" %c", &answer);

  if (answer == 'y' || answer == 'Y' || answer == 'S' || answer == 's')
  {
    printf("Nice.\n");
    //printf("\nCurrent:%s\n", current->text);
    return;
  }
  else
  {
    //printf("\nCurrent:%s\n", current->text);
    addNewAnswer(current);
    return;
  }
}

void saveNode(FILE *file, Ttree *node)
{
  if (node == NULL)
  {
    fprintf(file, "NULL\n");
  }
  else
  {
    fprintf(file, "%s\n", node->text);
    saveNode(file, node->left);
    saveNode(file, node->right);
  }
}

void saveTree(Ttree *tree, const char *filename)
{
  FILE *file = fopen(filename, "w");
  if (file == NULL)
    return;

  saveNode(file, tree);

  fclose(file);
}

int main()
{
  temp = (char *)malloc(32 * sizeof(char));
  correct_answer = (char *)malloc(32 * sizeof(char));
  difference = (char *)malloc(32 * sizeof(char));
  tree = create_tree("tree_data.txt");

  printf("Piensa en un objeto...\n");
  guessObject(tree);

  // ShowTree(tree, 2);
  saveTree(tree, "tree_data.txt");

  return 0;
}
