import tatodb
for i in range(100):
    print("new loop");
    monItem = tatodb.Item('fra',str(i));
    monItem.metas['prout'] = 'pouet';
    print(monItem.metas['prout']);
    monItem.metas;
