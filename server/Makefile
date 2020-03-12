objects = main.o Request.o Response.o Pollevent.o
myserver : $(objects)
	g++ -o myserver $(objects)

main.o : Response.h PollEvent.h
Request.o : Request.h 
Response.o: Response.h
Pollevent.o: PollEvent.h

.PHONY : clean
clean : 
	-rm myserver $(objects)
