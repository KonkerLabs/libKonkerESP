#ifndef __BUFFER_ENTRY_H__
#define __BUFFER_ENTRY_H__

#include "./protocols/protocol.h"

class BufferEntry {
  public: 
    ConnectionType connectionType;
    Protocol *protocol; 
    String payload;
    String channel;
};

#endif