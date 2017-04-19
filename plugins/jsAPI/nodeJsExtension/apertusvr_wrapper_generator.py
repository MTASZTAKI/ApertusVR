f1 = open('./apertusvr_wrap.cxx','r')
f2 = open('./apertusvr_wrap_replaced.cxx', 'w')
lines = f1.readlines()

# insert 'lock()->' before function names
# ---------------------------------------
# functionList = []
# for line in lines:
# 	if 'SWIGV8_AddMemberFunction(_exports_NodeWeakPtr_class' in line:
# 		functionName = line[line.index('"')+1:line.index('_wrap_')-3]
# 		functionList.append(functionName)
# print functionList
# for line in lines:
#	newline = line
#	if 'arg1)->' in line:
#		for fn in functionList:
#			if 'arg1)->'+fn+'(' in line:
#				newline = newline.replace('arg1)->',
#										  'arg1)->lock()->')
#				print newline

# replace INode stuff to NodeWeakPtr
# ---------------------------------------
for line in lines:
	newline = line
	newline = newline.replace('_exports_INode_clientData;',
							  '_exports_NodeWeakPtr_clientData;')
	newline = newline.replace('_wrap_new_veto_INode',
							  '_wrap_new_veto_NodeWeakPtr')
	newline = newline.replace('/* Class: INode (_exports_INode) */', 
							  '/* Class: NodeWeakPtr (_exports_NodeWeakPtr) */')
	newline = newline.replace('/* Name: _exports_INode, Type: p_Ape__INode, Dtor: 0 */',
							  '/* Name: _exports_NodeWeakPtr, Type: p_std__shared_ptrT_Ape__INode_t, Dtor: 0 */')
	newline = newline.replace('SWIGV8_CreateClassTemplate("INode");',
							  'SWIGV8_CreateClassTemplate("NodeWeakPtr");')
	newline = newline.replace('SWIGV8_CreateClassTemplate("_exports_INode");',
							  'SWIGV8_CreateClassTemplate("_exports_NodeWeakPtr");')
	newline = newline.replace('SWIGTYPE_p_Ape__INode->clientdata',
							  'SWIGTYPE_p_std__weak_ptrT_Ape__INode_t')
	newline = newline.replace('_exports_INode_class_0', 
							  '_exports_NodeWeakPtr_class_0')
	newline = newline.replace('_exports_INode_class', 
							  '_exports_NodeWeakPtr_class')
	newline = newline.replace('_exports_INode_obj', 
							  '_exports_NodeWeakPtr_obj')
	newline = newline.replace('_wrap_INode_', 
							  '_wrap_NodeWeakPtr_')
	newline = newline.replace('Ape::INode *arg1 = (Ape::INode *) 0;',
							  'Ape::NodeWeakPtr *arg1 = (Ape::NodeWeakPtr *) 0;')
	newline = newline.replace('arg1 = (Ape::INode *)(argp1);',
							  'arg1 = (Ape::NodeWeakPtr *)(argp1);')
	newline = newline.replace('result = ((Ape::INode const *)arg1)->',
							  'result = ((Ape::NodeWeakPtr const *)arg1)->lock()->')
	newline = newline.replace('SWIG_ConvertPtr(args.Holder(), &argp1, SWIGTYPE_p_Ape__INode, 0 | 0);',
							  'SWIG_ConvertPtr(args.Holder(), &argp1, SWIGTYPE_p_std__weak_ptrT_Ape__INode_t, 0 | 0);')
	newline = newline.replace('exports_obj->Set(SWIGV8_SYMBOL_NEW("INode"), _exports_NodeWeakPtr_obj);',
							  'exports_obj->Set(SWIGV8_SYMBOL_NEW("NodeWeakPtr"), _exports_NodeWeakPtr_obj);')
	print newline
	f2.write(newline)
f2.close()
