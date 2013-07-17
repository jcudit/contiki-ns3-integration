import socket
import time

UDP_IP="127.0.0.1"
UDP_PORT_BASE=65000
PATTERNS = list()
NUM_OUTPUT_LAYER_NODES=5
WSOCK = socket.socket( socket.AF_INET, socket.SOCK_DGRAM )
RSOCK = socket.socket( socket.AF_INET, socket.SOCK_DGRAM )
RSOCK.settimeout(1)
RSOCK.bind( (UDP_IP, 6565) )
MATRIX = [
[0,0,0,0,0,0,0,0], 
[0,0,0,0,0,0,0,0],
[0,0,0,0,0,0,0,0],
[0,0,0,0,0,0,0,0],
[0,0,0,0,0,0,0,0],
[0,0,0,0,0,0,0,0],
[0,0,0,0,0,0,0,0],
[0,0,0,0,0,0,0,0],
]

###########################################################################

# Set the input matrix to a predefined configuration
def setMatrix(v):
  if v == 'A':
    flipCell('13')
    flipCell('33')
    flipCell('14')
    flipCell('34')
    flipCell('22')
    flipCell('32')
    flipCell('42')
    flipCell('52')
    flipCell('62')
    flipCell('25')
    flipCell('35')
    flipCell('45')
    flipCell('55')
    flipCell('65')
  elif v == 'B':
    flipCell('22')
    flipCell('32')
    flipCell('42')
    flipCell('52')
    flipCell('62')
    flipCell('72')
    flipCell('13')
    flipCell('14')
    flipCell('43')
    flipCell('44')
    flipCell('73')
    flipCell('74')
    flipCell('25')
    flipCell('35')
    flipCell('55')
    flipCell('65')
  elif v == 'C':
    flipCell('22')
    flipCell('32')
    flipCell('42')
    flipCell('52')
    flipCell('13')
    flipCell('14')
    flipCell('15')
    flipCell('63')
    flipCell('64')
    flipCell('65')
  elif v == 'D':
    flipCell('12')
    flipCell('22')
    flipCell('32')
    flipCell('42')
    flipCell('52')
    flipCell('62')
    flipCell('13')
    flipCell('14')
    flipCell('63')
    flipCell('64')
    flipCell('25')
    flipCell('35')
    flipCell('45')
    flipCell('55')
  elif v == 'E':
    flipCell('12')
    flipCell('13')
    flipCell('14')
    flipCell('15')
    flipCell('22')
    flipCell('32')
    flipCell('33')
    flipCell('34')
    flipCell('42')
    flipCell('43')
    flipCell('44')
    flipCell('52')
    flipCell('62')
    flipCell('63')
    flipCell('64')
    flipCell('65')
  elif v == '0':
    for i in range(8):
      for j in range(8):
        if MATRIX[i][j] == 1:
          flipCell(str(i) + str(j))
  elif v == '1':
    for i in range(8):
      for j in range(8):
        if MATRIX[i][j] == 0:
          flipCell(str(i) + str(j))
 
# Display the current state of the sensor matrix
def displayMatrix():
  header = "    "
  for i in range(8):
    header += " " + str(i) + " "
  print header
  for i in range(8):
    line = str(i) + "   " 
    for j in range(8):
      if MATRIX[i][j]:
        line += " # "
      else:
        line += " . "
    print line

# Flip a cell in the sensory matrix and feed this into the neural network
def flipCell(rc):
  try:
    row = int(rc[0])
    column = int(rc[1])
  except BaseException:
    print "Bad Input.  Try again."
    return
  
  port = UDP_PORT_BASE + NUM_OUTPUT_LAYER_NODES + 8 * row + column 
  #print "Sending to cell " + rc + " port #" + str(port)
  if MATRIX[row][column]:
    MATRIX[row][column] = 0
  else:
    MATRIX[row][column] = 1
  WSOCK.sendto( str(MATRIX[row][column]), (UDP_IP, port) )

# Send via sensor to nodes that are beside (UDLR) trained-active nodes
def signalAdjacency(rc):
  try:
    row = int(rc[0])
    column = int(rc[1])
  except BaseException:
    print "Bad Input.  Try again."
    return

  port = UDP_PORT_BASE + NUM_OUTPUT_LAYER_NODES + 8 * row + column
  #print "Sending to cell " + rc +  " port #" + str(port)
  WSOCK.sendto( "T", (UDP_IP, port) )


# Query output layer for recognized pattern
def queryANN ():
  rec = 0 
  for i in range(NUM_OUTPUT_LAYER_NODES):
    WSOCK.sendto( "Q", (UDP_IP, UDP_PORT_BASE + i) )
    try:
      data = RSOCK.recv(1)
    except BaseException:
      data = '0'
      print "Socket Timeout, try again"
    if (data[0] == '1'):
      print "Recognized: " + PATTERNS[i]
      rec = 1 
  if rec != 1:
    print "No patterns recognized"

# Assign current input matrix state as a trained pattern
def trainANN (index,pattern):
  global PATTERNS 
  try:
    trainingIndex = int(index) - 1
  except BaseException:
    print "Bad Input.  Try again."
    return

  print "Training"
  # Choose an output node and place it in the Training state
  if trainingIndex > (NUM_OUTPUT_LAYER_NODES - 1) or trainingIndex < 0:
    print "Bad Training Index"
    return 
  WSOCK.sendto( "T", (UDP_IP, UDP_PORT_BASE + trainingIndex) )
  try:
    PATTERNS[trainingIndex] = pattern
  except BaseException:
    PATTERNS.append(pattern)
  # Signal nodes adjacent to an active node
  for i in range(8):
    for j in range(8):
      if MATRIX[i][j] == 1:
        if i - 1 >= 0 and MATRIX[i-1][j] != 1:
          signalAdjacency(str(i-1) + str(j))
        if i + 1 <= 7 and MATRIX[i+1][j] != 1:
          signalAdjacency(str(i+1) + str(j))
        if j - 1 >= 0 and MATRIX[i][j-1] != 1:
          signalAdjacency(str(i) + str(j-1))
        if j + 1 <= 7 and MATRIX[i][j+1] != 1:
          signalAdjacency(str(i) + str(j+1))
  for i in range(5):
    print "."
    time.sleep(1)
  print "Training Complete, Resetting Matrix"
  setMatrix("0")
  
###########################################################################

# Assign output layer nodes
print "Creating Neural Network Architecture"
for i in range(10):
  print "."
  time.sleep(1)
for i in range(NUM_OUTPUT_LAYER_NODES):
  WSOCK.sendto( "2", (UDP_IP, UDP_PORT_BASE + i) )
for i in range(10):
  print "."
  time.sleep(1)
print "Complete!"

while True:
  displayMatrix()
  ri = raw_input("Enter the cell you want to modify (i.e. 04 for the cell at row 0, column 4)\nEnter 'tN' to train, 'r' to recall or 'q' to quit: ")
  if ri[0] == 'q':
    raise SystemExit(0)
  elif ri[0] == 'r':
    queryANN()
  elif ri[0] == 't':
    pattern = raw_input("Describe the current pattern: ")
    trainANN(ri[1],pattern)
  elif ri[0] == 's':
    setMatrix(ri[1])
  else:
    flipCell(ri)
