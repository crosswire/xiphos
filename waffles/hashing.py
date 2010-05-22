# http://akiscode.com/articles/sha-1directoryhash.shtml
# Copyright (c) 2009 Stephen Akiki
# MIT License (Means you can do whatever you want with this)
#  See http://www.opensource.org/licenses/mit-license.php
# Error Codes:
#   -1 -> Directory does not exist
#   -2 -> General error (see stack traceback)

def GetHashofDirs(dirs, verbose=0):
  import hashlib, os
  SHAhash = hashlib.sha1()
    
  try:
    for directory in dirs:
      if not os.path.exists (directory):
        pass
      for files in os.listdir(directory):
        if verbose == 1:
          print 'Hashing', files
        filepath = os.path.join(directory,files)
        try:
          f1 = open(filepath, 'rb')
        except:
          # You can't open the file for some reason
          f1.close()
          continue

        while 1:
          # Read file in as little chunks
          buf = f1.read(4096)
          if not buf : break
          SHAhash.update(hashlib.sha1(buf).hexdigest())
        f1.close()

  except:
    import traceback
    # Print the stack traceback
    traceback.print_exc()
    return -2

  return SHAhash.hexdigest()
#print GetHashofDirs('My Documents', 1)
