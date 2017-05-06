#include "tools.h"


voListObject::voListObject(void)
{
  prev = next = NULL;
}

voListObject::~voListObject()
{
}

void voListObject::Append(voListObject *Object)
{
  next = Object;
  Object->prev = this;
}

void voListObject::Insert(voListObject *Object)
{
  prev = Object;
  Object->next = this;
}

void voListObject::Unlink(void)
{
  if (next)
     next->prev = prev;
  if (prev)
     prev->next = next;
  next = prev = NULL;
}

int voListObject::Index(void) const
{
  voListObject *p = prev;
  int i = 0;

  while (p) {
        i++;
        p = p->prev;
        }
  return i;
}

// --- voListBase -------------------------------------------------------------

voListBase::voListBase(void)
{
  objects = lastObject = NULL;
  count = 0;
}

voListBase::~voListBase()
{
  Clear();
}

void voListBase::Add(voListObject *Object, voListObject *After)
{
  if (After && After != lastObject) {
     After->Next()->Insert(Object);
     After->Append(Object);
     }
  else {
     if (lastObject)
        lastObject->Append(Object);
     else
        objects = Object;
     lastObject = Object;
     }
  count++;
}

void voListBase::Clear(void)
{
  while (objects) {
        voListObject *object = objects->Next();
        delete objects;
        objects = object;
        }
  objects = lastObject = NULL;
  count = 0;
}

voListObject *voListBase::Get(int Index) const
{
  if (Index < 0)
     return NULL;
  voListObject *object = objects;
  while (object && Index-- > 0)
        object = object->Next();
  return object;
}