  // Copyright 2012 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Author: tomasz.kaftal@gmail.com (Tomasz Kaftal)
//
// The file provides file management utilities.
#ifndef SUPERSONIC_OPENSOURCE_FILE_FILE_UTIL_H_
#define SUPERSONIC_OPENSOURCE_FILE_FILE_UTIL_H_

#include <functional>
#include <string>
#include "file/file.h"

namespace file_util {

using file::File;

// Join two path components, adding a slash if necessary.  If basename is an
// absolute path then JoinPath ignores dirname and simply returns basename.
std::string JoinPath(StringPiece dirname, StringPiece basename);

// Retrieve file name from a path. If path dosnt's contain dir returns the path itself.
StringPiece GetNameFromPath(StringPiece path);

// Tries to open the file under file_name in the given mode. Will fail if
// there are any errors on the way.
file::File* OpenOrDie(StringPiece file_name, StringPiece mode = StringPiece());

// Read an entire file to a std::string.  Return true if successful, false
// otherwise.
bool ReadFileToString(StringPiece name, std::string* output);

// Same as above, but crash on failure.
void ReadFileToStringOrDie(StringPiece name, std::string* output);

// Create a file and write a std::string to it.
void WriteStringToFileOrDie(StringPiece contents, StringPiece name);

// Create a directory.
bool CreateDir(StringPiece name, int mode);

// Create a directory and all parent directories if necessary.
bool RecursivelyCreateDir(StringPiece path, int mode);

// If "name" is a file, we delete it.  If it is a directory, we
// call DeleteRecursively() for each file or directory (other than
// dot and double-dot) within it, and then delete the directory itself.
// The "dummy" parameters have a meaning in the original version of this
// method but they are not used anywhere in protocol buffers.
void DeleteRecursively(StringPiece name);

void TraverseRecursively(StringPiece path, std::function<void(StringPiece)> cb);

int64_t LocalFileSize(StringPiece path);   // In bytes.

// Uses glob rules for local file system and the usual ls expansion for s3.
std::vector<std::string> ExpandFiles(StringPiece path);

// Similar to ExpandFiles but also returns statistics about files sizes and timestamps.
std::vector<file::StatShort> StatFiles(StringPiece path);
base::Status StatFilesSafe(StringPiece path, std::vector<file::StatShort>* res);

// Creates 'file.gz', compresses file, once successful, deletes it. fails on any error.
void CompressToGzip(StringPiece file, uint8_t compress_level = 2);

void CopyFileOrDie(StringPiece src, StringPiece dest_path);

// return false if path is file, true if dir, otherwise die.
bool IsFileDirOrDie(const std::string& path);


class TempFile {
 public:
  // Creates a file with a temporary-looking filename in read/write
  // mode in directory 'directory_prefix' (which can be followed or
  // not by a "/") or, if 'directory_prefix' is NULL or the empty string,
  // in a temporary directory (as provided by GetExistingTempDirectories()).
  //
  // Returns: a new File*, opened for read/write or NULL if it couldn't create
  // one.
  static file::File* Create(const char *directory_prefix);

  // The following method returns a temporary-looking filename. Be
  // advised that it might change behavior in the future and the
  // name generated might not follow any specific rule or pattern.
  //
  // Returns a unique filename in directory 'directory_prefix' (which
  // can be followed or not by a "/") or, if 'directory_prefix' is
  // NULL in a local scratch directory. Unique filenames are based on
  // localtime, cycle counts, hostname, pid and tid, to ensure
  // uniqueness. Returns: true if 'filename' contains a unique
  // filename, otherwise false (and 'filename' is left unspecified).
  static bool TempFilename(const char *directory_prefix, std::string *filename);

  // Similar as above but returns the file name rather than writing it to
  // an output argument.
  static std::string TempFilename(const char *directory_prefix);
};


// Creates a temporary directory on demand and deletes it when the process quits.
class TempDirDeleter {
 public:
  TempDirDeleter() {}
  ~TempDirDeleter();
  std::string GetTempDir();
 private:
  std::string name_;
};

};

#endif  // SUPERSONIC_OPENSOURCE_FILE_FILE_UTIL_H_
