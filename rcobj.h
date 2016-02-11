// -*- mode: C++; tab-width: 4; coding: utf-8 -*-
/*****
  rcobj.h: A small class to encode ok/ng/error in a machine word.

  Copyright: (C) 2016 "Hiroaki KOBAYASHI"

  Author: "Hiroaki KOBAYASHI" <hkoba@cpan.org>.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*****/

#ifndef __RCOBJ_H__
#define __RCOBJ_H__

/*
 * This code was originally written by myself in 2001.
 * At that time it was called "errobj.h".
 */

/*
 * class rcobj -- lightweight "return code" object for normal cases and errors.
 *
 * For normal cases, this object has value from 0 to 3, which encodes
 *
 *  bit 0: Empty-ness of result.
 *  bit 1: Incompleteness of the task. (ie. still readable, retry-able, blocked)
 *
 * In other words,
 *   0 means the function returend a result and it is finished.
 *   1 means it returned no result and it is finished.
 *   2 means it returend a result and you can still call it later.
 *   3 means it returned no result and you can try it later.
 *
 * For error cases, this object just hold "char *". This string should be
 * statically allocated in a similar manner of assert().
 *
 * This class assumes above error message string is not allocated
 * at 0..3 region. I hope this is true for you too;-)
 * 
 */

#ifndef __BEGIN_DECLS
#ifdef __cplusplus
#define __BEGIN_DECLS extern "C" {
#define __END_DECLS   }
#else
#define __BEGIN_DECLS
#define __END_DECLS
#endif
#endif

__BEGIN_DECLS

typedef struct rcobj_tag {
  union {
	const char* message;
	unsigned long code; // Expect LP64, ILP32. XXX: Not sure for IL32P64.
  } u;
} rcobj_t;

static inline int rcobj_has_no_results(const rcobj_t err) {
  return err.u.code >= 4 || ((err.u.code & 1) == 1);
}
static inline int rcobj_has_results(const rcobj_t err) {
  return err.u.code == 0 || (err.u.code < 4 && ((err.u.code & 1) == 0));
}
static inline int rcobj_has_error(const rcobj_t err) {
  return err.u.code >= 4;
}
static inline const char* rcobj_string(const rcobj_t err) {
  if (err.u.code >= 4)
	return err.u.message;
  switch (err.u.code) {
  case 0:  return "OK_DONE";
  case 1:  return "EOF";     // NG_DONE
  case 2:  return "OK_CONT";
  case 3:  return "BLOCKED"; // NG_CONT
  default: return "(UNKNOWN)";
  }
}

__END_DECLS

#ifdef __cplusplus
struct rcobj : public rcobj_t {
  // Bits for negative meanings.
  enum BIT {
	BIT_EMPTY = 0,
	BIT_INCOMPLETE = 1,
  };
  enum FLAG {
	FLAG_EXIST      = 0 << BIT_EMPTY,
	FLAG_EMPTY      = 1 << BIT_EMPTY,
	FLAG_DONE       = 0 << BIT_INCOMPLETE,
	FLAG_INCOMPLETE = 1 << BIT_INCOMPLETE,
	FLAG_RETRY = FLAG_INCOMPLETE,
	FLAG_CONT  = FLAG_RETRY,
  };
  enum CODE {
	// OK means: we got valid result.
	// NG means: we don't have valid result NOW. (but not a fatal case)

	OK_DONE = FLAG_EXIST | FLAG_DONE,
	OK_CONT = FLAG_EXIST | FLAG_CONT,
	NG_DONE = FLAG_EMPTY | FLAG_DONE,
	NG_CONT = FLAG_EMPTY | FLAG_CONT,

	// end marker.
	CODE_MAX,

	// aliases.
	OK_CONTINUE = OK_CONT,

	NG_CONTINUE = NG_CONT,
	NG_RETRY    = NG_CONT,
	NG_BLOCKED  = NG_CONT,
	BLOCKED     = NG_CONT,

	NG_EOF      = NG_DONE,
	FINISHED    = NG_DONE,
  };
  //
  rcobj() { u.message = "(not initialized)"; }
  // implicit
  rcobj(const CODE c) { u.code = c; }
  // implicit
  rcobj(const char* msg) {
	u.message = (char*) msg;
	if (msg)
	  breakpoint();
  }
  rcobj(const rcobj& orig) { u.message = orig.u.message; }
  rcobj& operator=(const rcobj& orig) {
	u.message = orig.u.message; return *this;
  }
  //
  bool is_error() const { return u.code >= CODE_MAX; }
  bool no_error() const { return u.code <  CODE_MAX; }
  // bool flag(int f) const { return (u.code & ~f) == f; }

  bool is_ON(const FLAG bit) const { return (u.code & bit) != 0; }
  bool is_OFF(const FLAG bit) const { return (u.code & bit) == 0; }

  bool has_results() const     { return no_error() && is_OFF(FLAG_EMPTY); }
  bool is_not_finished() const { return no_error() && is_ON(FLAG_INCOMPLETE);}
  bool has_no_results() const  { return is_error() || is_ON(FLAG_EMPTY); }
  bool is_finished() const     { return is_error() || is_OFF(FLAG_INCOMPLETE);}
  //
  bool is_ok() const { return has_results(); }
  bool is_ng() const { return has_no_results(); }
  bool is_done() const { return is_finished(); }
  bool is_blocked() const { return is_not_finished(); }
  //
  bool is_ok(rcobj& err) const { err = *this; return is_ok(); }
  bool is_ng(rcobj& err) const { err = *this; return is_ng(); }
  bool is_done(rcobj& err) const { err = *this; return is_done(); }
  bool is_blocked(rcobj& err) const { err = *this; return is_blocked(); }
  bool has_results(rcobj& err) const { err = *this; return has_results(); }
  bool has_no_results(rcobj& err) const {
	err = *this; return has_no_results();
  }
  bool has_no_results(rcobj* err) const { return has_no_results(*err);  }
  bool has_error() const { return is_error(); }
  bool has_error(const char** msg) const {
	if (! is_error()) {
	  *msg = 0;
	  return false;
	}
	*msg = u.message;
	return true;
  }

  // Debugging aid.
  static void breakpoint() {
	;
  }
  //
  const char* str() const { return is_error() ? u.message : 0; }
  // operator char*() const { return str(); } // (conflicts to op void*())
  const char* code_str() const {
	if (is_error())
	  return "ERROR";
	switch (u.code) {
	case OK_DONE: return "OK_DONE";
	case NG_EOF : return "NG_EOF";
	case OK_CONT: return "OK_CONT";
	case NG_BLOCKED: return "NG_BLOCKED";
	default:
	  return "(UNKNOWN)";
	}
  }
  //
  bool operator!() const { return is_ng(); }
  operator void*() const { return (void*) (is_ok() ? 1UL : 0UL); }
};

#define RCOBJ_STR(x) RCOBJ_XSTR(x)
#define RCOBJ_XSTR(x) #x

#define RCOBJ(m) \
  rcobj(m " in " __FILE__ ":" RCOBJ_STR(__LINE__))
#define RET_ERROR(m) return RCOBJ(m)

#define RET_OK_DONE return rcobj(rcobj::OK_DONE)
#define RET_OK_CONT return rcobj(rcobj::OK_CONT)
#define RET_OK_CONTINUE return rcobj(rcobj::OK_CONTINUE)

#define RET_NG_DONE  return rcobj(rcobj::NG_DONE)
#define RET_NG_RETRY return rcobj(rcobj::NG_RETRY)

#define RET_EOF     return rcobj(rcobj::NG_EOF)
#define RET_BLOCKED return rcobj(rcobj::BLOCKED)

#ifdef NDEBUG
#define RET_ASSERT(e)
#else//NDEBUG
#define RET_ASSERT(e) if (! (e)) RET_ERROR("Assertion '" #e "' failed")
#endif//NDEBUG

#endif//__cplusplus

#endif//__RCOBJ_H__
