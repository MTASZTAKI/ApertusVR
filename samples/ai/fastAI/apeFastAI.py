import sys
import requests
import fastai
import json
import PIL
from fastai import *
from fastai.vision import *
from fastai.metrics import error_rate
from fastai.vision.transform import *
import pretrainedmodels
import time
import math

np.random.seed(2)
data = ImageDataBunch.from_folder('resources/', train='train', valid='test',
									ds_tfms=get_transforms(flip_vert=True), 
									size=28,num_workers=0, bs=512).normalize(imagenet_stats)


API_ENDPOINT = "'http:/localhost'"
API_KEY = "XXXXXXXXXXXXXXXXX"
file_paths =  { "classes": 2, "IDs" : []}
leng = len('resources\test\\dog')
for i in data.valid_ds.items:
    file_paths["IDs"].append(str(i)[(leng+2):])
    #if str(i).find("Not")>0:
    #file_paths["IDs"].append(str(i)[(leng+2):])
    #else:
    #file_paths["IDs"].append(str(i)[(leng-1):])
PI = math.pi
PI_2 = PI / 2


file_paths2 =  { "classes": 2, "IDs" : file_paths["IDs"]}
try:
    r = requests.post(url = 'http://127.0.0.1:3000/api/v1/setfastai', json = file_paths2,timeout=0.5) 
    print(r.text)
    r.status_code
except:
    pass

   
allow_growth = True
def get_cadene_model(pretrained=True, model_name='se_resnet50'):
  if pretrained:
    arch = pretrainedmodels.__dict__[model_name](num_classes=1000, pretrained='imagenet')
  else:
    arch = pretrainedmodels.__dict__[model_name](num_classes=1000, pretrained=None)
  custom_head3 = nn.Sequential(AdaptiveConcatPool2d(), Flatten(), nn.BatchNorm1d(4096), 
                            nn.Linear(in_features=4096, out_features=512), nn.ReLU(),
                            nn.BatchNorm1d(512), nn.Linear(in_features=512, out_features=2),
                            nn.LogSoftmax())
  arch_2 = nn.Sequential(*list(children(arch))[:-2], custom_head3)
  return arch_2
learn3_cadene = cnn_learner(data, partial(get_cadene_model,model_name='se_resnext101_32x4d'),metrics=[accuracy, error_rate], cut=-2)

#learn3_cadene = learn3_cadene.load('fastai_weights/2fastai_112_black_fine_best')
learn3_cadene = cnn_learner(data, partial(get_cadene_model,model_name='se_resnext101_32x4d'),metrics=[accuracy, error_rate], cut=-2)


for i in range(10):
  try:
      learn3_cadene = learn3_cadene.load('./resources')
  except:
    pass
  learn3_cadene.fit_one_cycle(1,1e-3,callbacks=[callbacks.SaveModelCallback(learn3_cadene, every='improvement', monitor='accuracy', name='./resources')])
  #time.sleep(10)
  #learn3_cadene.save("/content/drive/My Drive/fastai_448_black{}".format(i))
  json_preds = { "x" : [], "y" : [], "z": []}
  preds,y,losses = learn3_cadene.get_preds(with_loss=True)  
  interp = ClassificationInterpretation(learn3_cadene,preds,y,losses)
  
  preds = preds.tolist()
  losses = losses.tolist()
  #print(y)
  #print(preds)
  maxloss = max(losses)
  k = 0;
  for pred in preds:
      if(pred[1] > pred[0]):
        phi = pred[1]*PI
        theta = (losses[k]/maxloss)*PI
        json_preds["x"].append(math.sin(phi)*math.cos(theta))
        json_preds["y"].append(math.sin(phi)*math.sin(theta))
        json_preds["z"].append(math.cos(phi))
      else:
        phi = pred[0]*PI-PI
        theta= (losses[k]/maxloss)*PI-PI
        json_preds["x"].append(math.sin(phi)*math.cos(theta))
        json_preds["y"].append(math.sin(phi)*math.sin(theta))
        json_preds["z"].append(math.cos(phi))
      k = k+1
  #print(json_preds)
  #sending = json.dumps(json_preds)
  try:
    r = requests.post(url = 'http://127.0.0.1:3000/api/v1/setfastai', json = json_preds,timeout=0.5) 
    print(r.text)
  except:
    pass
