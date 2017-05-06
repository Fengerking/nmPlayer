/************************************************************************
VisualOn Proprietary
Copyright (c) 2003, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/
#ifndef _TOOLS_H_
#define _TOOLS_H_

#include "voType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE
#endif

class voListObject {
private:
  voListObject *prev, *next;
public:
  voListObject(void);
  virtual ~voListObject();
  virtual int Compare(const voListObject &ListObject) const { return 0; }
  void Append(voListObject *Object);
  void Insert(voListObject *Object);
  void Unlink(void);
  int Index(void) const;
  voListObject *Prev(void) const { return prev; }
  voListObject *Next(void) const { return next; }
  };

class voListBase {
protected:
  voListObject *objects, *lastObject;
  voListBase(void);
  int count;
public:
  virtual ~voListBase();
  void Add(voListObject *Object, voListObject *After = NULL);
  virtual void Clear(void);
  voListObject *Get(int Index) const;
  int Count(void) const { return count; }
  };

template<class T> class voList : public voListBase {
public:
  T *Get(int Index) const { return (T *)voListBase::Get(Index); }
  T *First(void) const { return (T *)objects; }
  T *Last(void) const { return (T *)lastObject; }
  T *Prev(const T *object) const { return (T *)object->voListObject::Prev(); } 
  T *Next(const T *object) const { return (T *)object->voListObject::Next(); }
  };


#endif