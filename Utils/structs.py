# Generates struct declarations

FOClient =  { 0x21C : (1, 'byte GameState'),
              0x1CE4 : (4, 'int wmX'),
              0x1CE8 : (4, 'int wmY'),
              0x6BA0 : (4, 'CritterCl* self') }

CritterCl = { 0x00 : (4, 'int id'),
              0x04 : (2, 'short pid'),
              0x06 : (2, 'short hexX'),
              0x08 : (2, 'short hexY'),
              0x0A : (1, 'byte dir'),
              0x120 : (1, 'byte isPlayer'),
              0x12C : (4, 'int hp'),
              0x13C : (4, 'int xp'),
              0x140 : (4, 'int level'),
              0x1AC : (4, 'int dialogID'),
           	  0xFE8 : (4, 'DWORD flags'),
              0x1000 : (4, 'char** name')
              }

def write_struct_file (name, decl, f):
        f.write(("struct %s {\n" % name))
        off = 0
        i = 0;
        lasto = 0;
        lasts = 0;
        for o in sorted (decl.keys()):
                diff = ((o - lasto) - lasts);
                if(diff > 0):
                    f.write("\tchar pad_%d [%s];\n" % (i, hex(diff)))
                lasto = o
                lasts = decl[o][0]
                
                i = i + 1
                off = off + decl[o][0]
                f.write("\t%s; // 0x%x \n" % (decl[o][1], o))
        f.write("};\n")

f = open('structs.txt','w')
write_struct_file("FOClient", FOClient, f)
write_struct_file("CritterCl", CritterCl, f)
f.close()
