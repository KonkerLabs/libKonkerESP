#include "buffer_entry.h"

// Type; {Payload}; Protocol; Channel
// Total bytes: 142 (144 when aligned)
// typedef struct buffer_entry
// {
//   uint8_t type;
//   char payload[120];
//   uint8_t protocol;
//   char channel[20];
// } BufferElement;

BufferEntry::BufferEntry()
{
  dataBuffer = (BufferElement*)malloc(sizeof(BufferElement) * BUFFER_SIZE);
}

BufferEntry::BufferEntry(ConnectionType type)
{
  dataBuffer = (BufferElement*)malloc(sizeof(BufferElement) * BUFFER_SIZE);
  this->connectionType = type;
}

BufferEntry::~BufferEntry()
{
  free(dataBuffer);
}

void BufferEntry::setConnectionType(ConnectionType type)
{
  this->connectionType = type;
}

void BufferEntry::incrementPosition(int * position)
{
  Log.trace("[Buffer] Incrementing: %d\n", *position);
  if(*position == BUFFER_SIZE - 1)
  {
    *position = 0;
  }
  else
  {
    *position += 1;
  }
}

void BufferEntry::decrementPosition(int * position)
{
  if(*position == 0)
  {
    *position = BUFFER_SIZE - 1;
  }
  else
  {
    *position -= 1;
  }
}

int BufferEntry::collectData(String channel, String payload)
{
  BufferElement * el = &dataBuffer[this->currentPosition];
  int ret = 1;

  el->type = 1;
  strcpy(el->payload, payload.c_str());
  el->retries = 0;
  el->protocol = (uint8_t)this->connectionType;
  strcpy(el->channel, channel.c_str());

  Log.trace("[Buffer] Add element %s @ %d\n", toString(el).c_str(), this->currentPosition);
  // dataBuffer[this->currentPosition] = el;

  // case current data overrides older data, which is lost
  if(((this->currentPosition + 1 == this->firstPosition) || (this->currentPosition + 1 == BUFFER_SIZE && this->firstPosition == 0)) && (!this->empty))
  {
    Log.warning("[Buffer] Overwriting older data from buffer!\n");
    incrementPosition(&this->firstPosition);
    ret = 0;
  }

  incrementPosition(&this->currentPosition);

  if(this->empty)
  {
    this->empty = false;
  }

  return ret;
}

BufferElement BufferEntry::consumeData()
{
  BufferElement el;
  memset(&el, 0, sizeof(BufferElement));

  if(this->empty)
  {
    Log.warning("[Buffer] Empty buffer! Returning null.\n");
  }
  else
  {
    el = dataBuffer[this->firstPosition];

    Log.trace("[Buffer] Removing data from buffer: %s @ %d\n", toString(&el).c_str(), this->firstPosition);
    // removeData(this->firstPosition);

    // case only one element left in buffer
    if (this->firstPosition == this->currentPosition)
    {
      this->firstPosition = 0;
      this->currentPosition = 0;
      this->empty = true;
    }
    else
    {
      incrementPosition(&this->firstPosition);
    }
  }

  return el;
}

int BufferEntry::getData(BufferElement *data)
{
  memset(data, 0, sizeof(BufferElement));

  if(this->empty)
  {
    Log.warning("[Buffer] Empty buffer! Returning null.\n");
    return -1;
  }
  else
  {
    *data = dataBuffer[this->firstPosition];

    Log.trace("[Buffer] Getting data from buffer: %s @ %d\n", toString(data).c_str(), this->firstPosition);
    // removeData(this->firstPosition);
  }

  return this->firstPosition;
}

void BufferEntry::removeData(int position)
{
  if(this->firstPosition == position)
  {
    // case only one element left in buffer
    if (this->firstPosition + 1 == this->currentPosition)
    {
      this->firstPosition = 0;
      this->currentPosition = 0;
      this->empty = true;
      Log.trace("[Buffer] Is empty\n");
    }
    else
    {
      incrementPosition(&this->firstPosition);
    }
  }
  else
  {
    // its not possible to remove element from middle of buffer at this time
    Log.error("[Buffer] Operation not allowed\n");
  }
}

void BufferEntry::clearDataBuffer()
{
  Log.notice("[Buffer] Removing all data from buffer!\n");
  this->firstPosition = 0;
  this->currentPosition = 0;
  this->empty = true;
}

void BufferEntry::printBufferStatus()
{
  Log.trace("------------------\n");
  Log.trace("[Buffer] First position = %d\n", this->firstPosition);
  Log.trace("[Buffer] Current position = %d\n", this->currentPosition);
  Log.trace("[Buffer] Empty = %d\n", this->empty);
  Log.trace("------------------\n");
}

bool BufferEntry::isEmpty()
{
  return this->empty;
}

void BufferEntry::incrementRetries(int position)
{
  if(dataBuffer[position].retries == this->maxRetries)
  {
    Log.warning("[Buffer] Maximum number of retries to send data exceeded\n");
    Log.warning("[Buffer] Removing data from buffer\n");
    removeData(position);
  }
  else
  {
    dataBuffer[position].retries++;
  }
}

String BufferEntry::toString(BufferElement * element)
{
  String ret = "T:" + String(element->type) + ";P:" + String(element->payload) + 
                ";L:" + element->protocol + ";C:" + element->channel;

  return ret;
}
