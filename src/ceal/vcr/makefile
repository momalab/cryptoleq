#all:
#	@echo "Usage: init|clean"

init: pqkru db.sec input.sec

pqkru:
	cp ../pqkru ./
	chmod 0777 pqkru

db.sec:
	cd ..; ./crypter enc db.opn vc/db.sec

input.sec:
	cd ..; ./crypter enc input.opn vc/input.sec

clean:
	rm -f db.sec input.sec output.sec output.opn pqkru db.opn input.opn
	rm -rf        ipch
	rm -rf        vc.sdf
	rm -rf        Debug
	rm -rf        Release
	rm -rf        DebugStatic
	rm -rf        vc.suo vc.*.suo
