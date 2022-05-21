#pragma once

static const int EIO    = 5;  // I/O error
static const int ENOMEM = 12; // Cannot allocate memory
static const int EINVAL = 22; // Invalid argument
static const int ENAMETOOLONG = 36; // path or filename too long
// TODO(tn259) this has no equivalent in linux
static const int EFSNOTPRESENT = 134; // FS not found on disk