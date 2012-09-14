#include <SFML/Network.hpp>
#include <iostream>
#include <vector>

using namespace std;

struct GameClient{
	sf::TcpSocket socket;
	int clientID;
};

int clientIDCounter = 100;

vector<GameClient*> clients;

//PACKET COMMAND CODES
const int YOURCLIENTID		= 999;
const int PLAYERLIST		= 1000;
const int NEWPLAYER			= 1001;
const int PLAYERDISCONNECT	= 1002;
const int POSITIONUPDATE	= 1003;

int main(){
	//Set up a listening socket
	cout << "Starting server on port 1337\n";
	sf::TcpListener listener;
	if( !listener.listen(1337) ){
		cout << "Unable to listen on port 1337!\n";
	}else{
		//Create a selector object
		//This will allow us to wait for any socket to be ready
		sf::SocketSelector selector;
		selector.add(listener);

		clients.clear();

		bool running = true;
		while(running){
			//Wait for a socket to be ready
			selector.wait();
			//If its the listener, then a new client is connecting
			if( selector.isReady(listener) ){
				GameClient *client = new GameClient;
				if( listener.accept(client->socket) != sf::Socket::Done ){
					cout << "Error while accepting client.\n";
					delete client;
				}else{
					selector.add(client->socket);
					client->clientID = clientIDCounter++;

					cout << "Client connected from " << client->socket.getRemoteAddress() << endl;

					//Send the other players a notification
					sf::Packet newPlayerPak;
					newPlayerPak << NEWPLAYER << client->clientID;
					for( unsigned int i = 0; i < clients.size(); ++i ){
						clients[i]->socket.send(newPlayerPak);
					}

					//Now add the player to the list
					clients.push_back(client);

					//Send the player his new ID
					sf::Packet idPak;
					idPak << YOURCLIENTID << client->clientID;
					client->socket.send(idPak);

					//Send the player the list of other players
					unsigned int playerCount = clients.size();
					sf::Packet playerPak;
					playerPak << PLAYERLIST << playerCount;
					for( unsigned int i = 0; i < playerCount; ++i ){
						playerPak << clients[i]->clientID;
					}
					client->socket.send(playerPak);
				}
			}else{
				for( unsigned int i = 0; i < clients.size(); ++i ){
					if( selector.isReady(clients[i]->socket) ){
						sf::TcpSocket& socket = clients[i]->socket;

						sf::Packet packet;
						if( socket.receive(packet) != sf::Socket::Done ){
							cout << "Error on a socket. (Client disconnected?)\n";
							selector.remove(socket);
							socket.disconnect();

							int clientID = clients[i]->clientID;
							clients.erase(clients.begin()+i);
							--i; //so the index is fixed after the loop

							//Send the other players a notification
							sf::Packet dcPak;
							dcPak << PLAYERDISCONNECT << clientID;
							for( unsigned int j = 0; j < clients.size(); ++j ){
								clients[j]->socket.send(dcPak);
							}

						}else{
							//Send the packet to all other clients
							for( unsigned int j = 0; j < clients.size(); ++j ){
								if( j != i ){ //Do not send to the one we received from
									clients[j]->socket.send(packet);
								}
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
