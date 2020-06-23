#ifndef __BUFFER_ENTRY_H__
#define __BUFFER_ENTRY_H__

#include "globals.h"
#include "protocol.h"

#define DEFAULT_MAX_RETRIES 10

// Type; {Payload}; U; Protocol; Channel
// Total bytes: 143 (144 when aligned)
typedef struct buffer_entry
{
  uint8_t type;
  char payload[120];
  uint8_t U;
  uint8_t protocol;
  uint8_t retries;
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

  int maxRetries = DEFAULT_MAX_RETRIES;

  void incrementPosition(int * position);
  void decrementPosition(int * position);

  String toString(BufferElement element);
public:
  BufferEntry();
  BufferEntry(ConnectionType type);
  ~BufferEntry();

  void setConnectionType(ConnectionType type);

  // add data in currentPosition
  int collectData(String channel, String payload);
  // get and remove data from firstPosition
  BufferElement consumeData();
  // get data from firstPosition, returns the position and fills data with it
  // returns -1 if buffer is empty
  int getData(BufferElement *data);
  // remove data from position if it's equals to firstPosition
  void removeData(int position);

  void incrementRetries(int position);

  void bufferStatus();
  bool isEmpty();
  void clearDataBuffer();
};

#endif
