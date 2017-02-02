file:
	g++ -o ftaclient ftaclient.cc
	g++ -o ftaserver ftaserver.cc
	g++ -o dbengineRTP dbengineRTP.cc
	g++ -o dbclientRTP dbclientRTP.cc
clean:
	rm ftaclient ftaserver dbengineRTP dbclientRTP

