#include <SFML/Network.hpp>
#include <iostream>
#include <vector>

using namespace std;

struct GameClient{
	sf::SocketTCP socket;
	int clientID;
};

int clientIDCounter = 100;

vector<GameClient> clients;

//PACKET COMMAND CODES
const int YOURCLIENTID		= 999;
const int PLAYERLIST		= 1000;
const int NEWPLAYER			= 1001;
const int PLAYERDISCONNECT	= 1002;
const int POSITIONUPDATE	= 1003;

int main(){
	//Set up a listening socket
	cout << "Starting server on port 1337\n";
	sf::SocketTCP listener;
	if( !listener.Listen(1337) ){
		cout << "Unable to listen on port 1337!\n";
	}else{
		//Create a selector object
		//This will allow us to wait for any socket to be ready
		sf::SelectorTCP selector;
		selector.Add(listener);
		
		clients.clear();

		bool running = true;
		while(running){
			//Wait for a socket to be ready
			unsigned int readySocketsCount = selector.Wait();
			for( unsigned int i = 0; i < readySocketsCount; ++i ){
				sf::SocketTCP socket = selector.GetSocketReady(i);

				//If its the listener, then a new client is connecting
				if( socket == listener ){
					GameClient client;
					sf::IPAddress clientAddr;
					if( listener.Accept(client.socket, &clientAddr) != sf::Socket::Done ){
						cout << "Error while accepting client.\n";
					}else{
						selector.Add(client.socket);
						client.clientID = clientIDCounter++;

						cout << "Client connected from " << clientAddr << endl;

						//Send the other players a notification
						sf::Packet newPlayerPak;
						newPlayerPak << NEWPLAYER << client.clientID;
						for( unsigned int i = 0; i < clients.size(); ++i ){
							clients[i].socket.Send(newPlayerPak);
						}

						//Now add the player to the list
						clients.push_back(client);

						//Send the player his new ID
						sf::Packet idPak;
						idPak << YOURCLIENTID << client.clientID;
						client.socket.Send(idPak);

						//Send the player the list of other players
						unsigned int playerCount = clients.size();
						sf::Packet playerPak;
						playerPak << PLAYERLIST << playerCount;
						for( unsigned int i = 0; i < playerCount; ++i ){
							playerPak << clients[i].clientID;
						}
						client.socket.Send(playerPak);
					}
				}else{
					//A client has sent data
					sf::Packet packet;
					if( socket.Receive(packet) != sf::Socket::Done ){
						cout << "Error on a socket. (Client disconnected?)\n";
						selector.Remove(socket);

						int closeClientID = 0;
						for( vector<GameClient>::iterator iter = clients.begin(); iter != clients.end(); ++iter ){
							if( iter->socket == socket ){
								closeClientID = iter->clientID;
								clients.erase(iter);
								break;
							}
						}
						socket.Close();

						//Send the other players a notification
						sf::Packet dcPak;
						dcPak << PLAYERDISCONNECT << closeClientID;
						for( unsigned int i = 0; i < clients.size(); ++i ){
							clients[i].socket.Send(dcPak);
						}

					}else{
						//Send the packet to all other clients
						for( unsigned int i = 0; i < clients.size(); ++i ){
							if( clients[i].socket != socket ){ //Do not send to the one we received from
								clients[i].socket.Send(packet);
							}
						}
					}
				}
			}

		}
	}
	cout << "Press any key to quit.\n";
	cin.sync();
	cin.ignore();
	return 1;
}