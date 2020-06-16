#ifndef __BUFFER_ENTRY_H__
#define __BUFFER_ENTRY_H__

#include "globals.h"
#include "protocol.h"

// Type; {Payload}; U; Protocol; Channel
// Total bytes: 143 (144 when aligned)
typedef struct buffer_entry
{
  uint8_t type;
  char payload[120];
  uint8_t U;
  uint8_t protocol;
  char channel[20];
} BufferElement;

class BufferEntry
{
private:
  ConnectionType connectionType;

  BufferElement * dataBuffer;
  int firstPosition = 0;
  // element inserted in currentPosition, incremented after
  int currentPosition = 0;
  bool empty = true;

  void incrementPosition(int * position);
  void removeData(int position);

  String toString(BufferElement element);
public:
  BufferEntry();
  BufferEntry(ConnectionType type);
  ~BufferEntry();

  void setConnectionType(ConnectionType type);

  // add data in currentPosition
  int collectData(String channel, String payload);
  // get data from firstPosition
  BufferElement consumeData();

  void bufferStatus();
  void clearDataBuffer();
};

#endif
