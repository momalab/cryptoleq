del db.sec input.sec output.sec output.opn out
crypter enc db.opn db.sec
crypter enc input.opn input.sec
prog_sec | dos2unix | tee out
crypter dec output.sec output.opn
