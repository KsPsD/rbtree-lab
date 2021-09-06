#include "rbtree.h"

#include <malloc.h>

bool isLeaf(const node_t *);
void bindNode(node_t *delp);
bool borrowKey(rbtree *t, node_t *delgp, node_t *delp, node_t *del, node_t *sib);
key_t swapKey(node_t *del);
bool redAsP(rbtree *t, node_t *delgp, node_t *delp, node_t *sib);
bool delLeaf(rbtree *t, const key_t key, node_t *delp, node_t *del);
bool is2node(const node_t *p);
bool isLeaf(const node_t *p);

rbtree *new_rbtree(void)
{
  rbtree *p = (rbtree *)calloc(sizeof(rbtree), 1);

  return p;
}

void deleteAll(node_t *p)
{
  if (p != NULL)
  {
    deleteNode(p->left);
    deleteNode(p->right);
    free(p);
  }
}

void delete_rbtree(rbtree *t)
{
  // TODO: reclaim the tree nodes's memory
  deleteAll(t->root);
  free(t);
}
node_t *rbtree_rotate(rbtree *t, node_t *pivot, const key_t key)
{
  node_t *child, *gchild;

  if ((key > pivot->key || key == pivot->key) && pivot != t->root)
    child = (node_t *)pivot->right;
  else
    child = (node_t *)pivot->left;

  //rotate left
  if (key > child->key || key == child->key)
  {
    gchild = (node_t *)child->right;
    child->right = gchild->left;
    gchild->left = (node_t *)child;
  }
  //rotate right
  else
  {
    gchild = (node_t *)child->left;
    child->left = gchild->right;
    gchild->right = (node_t *)child;
  }

  if ((key > pivot->key || key == pivot->key) && pivot != t->root)
    pivot->right = gchild;
  else
    pivot->left = gchild;
  return gchild;
}

node_t *rbtree_insert(rbtree *t, const key_t key)
{
  // TODO: implement insert

  node_t *target, *p, *gp, *ggp;
  if (t->root == NULL)
  {
    t->root = (node_t *)malloc(sizeof(node_t));
    t->root->color = RBTREE_BLACK;
    t->root->left = 0;
    t->root->right = 0;
  }

  ggp = gp = p = (node_t *)t->root;
  target = (node_t *)t->root->left;

  while (target)
  {
    if (key == target->key)
      return NULL;
    if (target->left && target->right && target->left->color == RBTREE_RED && target->right->color == RBTREE_RED)
    {
      target->color = RBTREE_RED;
      target->left->color = target->right->color = RBTREE_RED;
      if (p->color == RBTREE_RED)
      {
        gp->color = RBTREE_RED;
        if ((key > gp->key) != (key > p->key))
          p = rbtree_rotate(t, gp, key);
        target = rbtree_rotate(t, ggp, key);
        target->color = RBTREE_BLACK;
      }
      t->root->left->color = RBTREE_BLACK;
    }
    ggp = gp;
    gp = p;
    p = target;
    if (key > target->key)
      target = target->right;
    else
      target = target->left;
  }

  target = (node_t *)calloc(sizeof(node_t), 1);
  target->key = key;
  target->left = target->right = 0;
  target->color = RBTREE_RED;
  if (key > p->key && p != t->root)
    p->right = target;
  else
    p->left = target;

  // t->m_cnt;
  if (p->color == RBTREE_RED)
  {
    gp->color = RBTREE_RED;
    if ((key > gp->key) != (key > p->key))
      p = rbtree_rotate(t, gp, key);
    target = rbtree_rotate(t, ggp, key);
    target->color = RBTREE_RED;
  }

  t->root->left->color = RBTREE_BLACK;

  return target;
}

node_t *rbtree_find(rbtree *t, const key_t key)
{
  // TODO: implemoent find
  node_t *s;
  s = t->root->left;
  while (s && !(key == s->key))
  {
    if (key > s->key)
      s = s->right;
    else
      s = s->left;
  }

  if (s == 0)
    return NULL;

  return s;
}

node_t *rbtree_min(rbtree *t)
{
  // TODO: implement find
  node_t *cur;
  cur = t->root->left;
  while (cur && cur->left != NULL)
  {
    cur = cur->left;
  }

  return cur;
}

node_t *rbtree_max(rbtree *t)
{
  // TODO: implement find
  return t->root;
}

bool isLeaf(const node_t *p)
{
  if (p == 0)
    return false;
  //왼쪽 자식 없거나 있으면 왼쪽 자식의 색이 빨강 그리고 자식들 없어야됨 오른쪽도 마찬가지
  if ((p->left == 0 || (p->left && p->left->color == RBTREE_RED && !p->left->left && !p->left->right)) && ((p->right == 0 || (p->right && p->right->color == RBTREE_BLACK && p->right->right && p->right->right))))
    return true;
  else
    return false;
}
bool is2node(const node_t *p)
{
  if (p == 0)
    return false;
  if (p->color == RBTREE_RED)
    return false;
  if ((p->left == 0 && p->right == 0) || (p->left && p->right && p->left->color == RBTREE_BLACK && p->right->color == RBTREE_BLACK))
    return true;
  else
    return false;
}
bool delLeaf(rbtree *t, const key_t key, node_t *delp, node_t *del)
{
  if (key == del->key && !del->left && !del->right)
  {
    free(del);
    if ((key > delp->key || key == delp->key) && delp != t->root)
      delp->right = 0;
    else
      delp->left = 0;

    return true;
  }
  else if (key == del->key)
  {
    node_t *ret;
    if (del->left)
    {
      del->left->right = del->right;
      ret = del->left;
      ret->color = RBTREE_BLACK;
      free(del);
    }
    else if (del->right)
    {
      del->right->left = del->left;
      ret = del->right;
      ret->color = RBTREE_BLACK;
      free(del);
    }
    if ((ret->key > delp->key || ret->key == delp->key) && delp != t->root)
      delp->right = ret;
    else
      delp->left = ret;
    return true;
  }
  else if (del->left && key == del->left->key)
  {
    free(del->left);
    del->left = 0;
    return true;
  }
  else if (del->right && key == del->right->key)
  {
    free(del->right);
    del->right = 0;
    return true;
  }
  else
    return false;
}
bool redAsP(rbtree *t, node_t *delgp, node_t *delp, node_t *sib)
{
  if (sib == 0 || sib->color == RBTREE_BLACK)
    return false;
  rbtree_rotate(t, delgp, sib->key);
  sib->color = RBTREE_BLACK;
  delp->color = RBTREE_RED;
  return true;
}
key_t swapKey(node_t *del)
{
  node_t *target;
  target = del->right;
  while (target->left)
    target = target->left;
  del->key = target->key;
  return target->key;
}

//형제에게서 빨간색을 빌려옴// del 검정, 형제 자식이 빨간색이 있어야됨
bool borrowKey(rbtree *t, node_t *delgp, node_t *delp, node_t *del, node_t *sib)
{
  node_t *sibrc;
  if (is2node(sib) == true)
    return false;
  if (del->key > sib->key)
  {
    if (sib->left && sib->left->color == RBTREE_RED)
      sibrc = sib->left;
    else
      sibrc = sib->right;
  }
  else
  {
    if (sib->right && sib->right->color == RBTREE_RED)
      sibrc = sib->right;
    else
      sibrc = sib->left;
  }

  if ((delp->key > sib->key) != (sib->key > sibrc->key))
  {
    rbtree_rotate(t, delp, sibrc->key);
    rbtree_rotate(t, delgp, sibrc->key);
    sib->color = RBTREE_BLACK;
    sibrc->color = RBTREE_RED;
  }
  else
  {
    rbtree_rotate(t, delp, sibrc->key);
    sib->color = RBTREE_RED;
    sibrc->color = RBTREE_BLACK;
  }

  delp->color = RBTREE_BLACK;
  del->color = RBTREE_RED;

  if (t->root->left->color)
    t->root->left->color = RBTREE_RED;
  return true;
}
void bindNode(node_t *delp)
{
  delp->color = RBTREE_BLACK;
  delp->left->color = RBTREE_RED;
  delp->right->color = RBTREE_RED;
}

int rbtree_erase(rbtree *t, node_t *p)
{
  // TODO: implement erase
  node_t *delgp, *delp, *del, *sib;
  key_t value = p->key;
  delgp = delp = t->root;
  del = t->root->left;
  sib = 0;

  while (isLeaf(del) == false)
  {
    if (del->color == RBTREE_BLACK)
    {
      if (redAsP(t, delgp, delp, sib) == true)
      {
        delgp = sib;
        if (del->key > delp->key || del->key == delp->key)
          sib = delp->left;
        else
          sib = delp->right;
      }

      if (del != t->root->left && is2node(del) == true)
      {
        if (borrowKey(t, delgp, delp, del, sib) == false)
          bindNode(delp);
      }
      if (value > del->key || value == del->key)
      {
        sib = del->left;
        del = del->right;
      }
      else
      {
        sib = del->right;
        del = del->left;
      }
    }
  }

  if (del->color == RBTREE_BLACK)
  {
    if (redAsP(t, delgp, delp, sib))
    {
      delgp = sib;
      if (del->key > delp->key || del->key == delp->key)
        sib = delp->left;
      else
        sib = delp->right;
    }
    if (del != t->root->left && is2node(del) == true)
    {
      if (borrowKey(t, delgp, delp, del, sib) == false)
        bindNode(delp);
    }
    if (delLeaf(t, value, delp, del))
    {
      t->m_cnt--;
      return true;
    }
  }

  return false;
}

int rbtree_to_array(rbtree *t, key_t *arr, const size_t n)
{
  // TODO: implement to_array
  return 0;
}
