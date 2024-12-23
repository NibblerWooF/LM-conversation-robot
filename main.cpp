#include "Web_Scr_set.h"
#include "servo.h"


#define key 0      
#define led 8       
#define light 38    

#define I2S_DOUT 5
#define I2S_BCLK 6 
#define I2S_LRC 7  




int llm = 0;    
String llm_name[] = {"豆包大模型", "讯飞星火大模型", "通义千问大模型", "通义千问智能体应用", "Chatgpt大模型", "Dify"};


String model1 = "";  
const char* doubao_apiKey = "";    
String apiUrl = "";


String APPID = "";                           
String APISecret = ""; 
String APIKey = "";    
String appId1 = APPID;
String domain1 = "";   
String websockets_server = "";   
String websockets_server1 = "";         



String roleSet = "你的名字叫小黑";
String roleSet1 = "你的名字叫小威";


unsigned long urlTime = 0;

int noise = 50;                 
int volume = 100;               



String url = "";
String url1 = "";
String Date = "";

String askquestion = "";        
String Answer = "";             
std::vector<String> subAnswers; 
int subindex = 0;               
String text_temp = "";          
int loopcount = 0;      
int flag = 0;           
int start_con = 0;      
int image_show = 0;
String conversation_id = "";
String session_id = "";

using namespace websockets; 
WebsocketsClient webSocketClient;   
WebsocketsClient webSocketClient1;  


Audio1 audio1;
Audio2 audio2(false, 3, I2S_NUM_1); 



DynamicJsonDocument gen_params(const char *appid, const char *domain);
DynamicJsonDocument gen_params_http(const char *model, const char *role_set);
DynamicJsonDocument gen_params_dify();
DynamicJsonDocument gen_params_tongyi_app();
void processResponse(int status);
void displayWrappedText(const string &text1, int x, int y, int maxWidth);
void getText(String role, String content);
void checkLen();
void removeChars(const char *input, char *output, const char *removeSet);
float calculateRMS(uint8_t *buffer, int bufferSize);
void ConnServer();
void ConnServer1();
void voicePlay();
int wifiConnect();
void getTimeFromServer();
String getUrl(String server, String host, String path, String date);
void doubao();
void tongyi();
void tongyi_app();
void chatgpt();
void dify();

void voicePlay()
{
    if ((audio2.isplaying == 0) && (Answer != "" || subindex < subAnswers.size()))
    {
        if (subindex < subAnswers.size())
        {
            audio2.connecttospeech(subAnswers[subindex].c_str(), "zh");
            if (text_temp != "" && flag == 1)
            {
                tft.fillScreen(TFT_WHITE);
                displayWrappedText(text_temp.c_str(), 0, 11, width);
                text_temp = "";
                displayWrappedText(subAnswers[subindex].c_str(), u8g2.getCursorX(), u8g2.getCursorY(), width);
            }
            else if (flag == 1)
            {
                tft.fillScreen(TFT_WHITE);
                displayWrappedText(subAnswers[subindex].c_str(), 0, 11, width);
            }
            subindex++;
        }
        else
        {
            audio2.connecttospeech(Answer.c_str(), "zh");
            if (text_temp != "" && flag == 1)
            {
                tft.fillScreen(TFT_WHITE);
                displayWrappedText(text_temp.c_str(), 0, 11, width);
                text_temp = "";
                displayWrappedText(Answer.c_str(), u8g2.getCursorX(), u8g2.getCursorY(), width);
            }
            else if (flag == 1)
            {
                tft.fillScreen(TFT_WHITE);
                displayWrappedText(Answer.c_str(), 0, 11, width);
            }
            Answer = "";
            conflag = 1;
        }
        startPlay = true;
    }
    else if (audio2.isplaying == 0 && musicplay == 1)  
    {
        preferences.begin("music_store", true);
        int numMusic = preferences.getInt("numMusic", 0);
        musicnum = musicnum + 1 < numMusic ? musicnum + 1 : 0;
        
        String musicName = preferences.getString(("musicName" + String(musicnum)).c_str(), "");
        String musicID = preferences.getString(("musicId" + String(musicnum)).c_str(), "");
        Serial.println("音乐名称: " + musicName);
        Serial.println("音乐ID: " + musicID);

        String audioStreamURL = "https:
        Serial.println(audioStreamURL.c_str());
        audio2.connecttohost(audioStreamURL.c_str());
        
        tft.fillRect(0, cursorY, width, 50, TFT_WHITE);
        askquestion = "正在顺序播放所有音乐，当前正在播放：" + musicName;
        Serial.println(askquestion);
        
        displayWrappedText(askquestion.c_str(), 0, cursorY + 11, width);
        askquestion = "";
        preferences.end();
        startPlay = true;
    }
    else
    {
       
    }
}

 
void imageshow()
{
    tft.fillScreen(TFT_WHITE);
    int count = 2;
    while (count)
    {
        for (int i = 0;i < bizhi_size;i++)
        {
            tft.pushImage(0, 0, width, height, bizhi[i]);  
            for (int j=0;j<100;j++)     
            {
                audio2.loop();
                delay(10);
            }
        }
        count--;
    }
    image_show = 0;
}

void setup()
{
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, 19, 20);
    pinMode(key, INPUT_PULLUP);
    pinMode(led, OUTPUT);
    pinMode(light, OUTPUT);

    Serial.println("引脚初始化完成！");

    
    tft.init();
    tft.setRotation(0);        
    tft.setSwapBytes(true);
    tft.fillScreen(TFT_WHITE);   
    tft.setTextColor(TFT_BLACK); 
    tft.setTextWrap(true);  

    
    u8g2.begin(tft);
    u8g2.setFont(u8g2_font_wqy12_t_gb2312); 
    u8g2.setFontMode(1);                    
    u8g2.setForegroundColor(TFT_BLACK);     
    u8g2.setCursor(0, 11);
    u8g2.print("已开机！");

    Serial.println("屏幕初始化完成！");

    audio1.init();
    audio2.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio2.setVolume(volume);


    preferences.begin("wifi_store");
    preferences.begin("music_store");
    u8g2.setCursor(0, u8g2.getCursorY() + 12);
    u8g2.print("正在连接网络······");
    int result = wifiConnect();

    if (result == 1)
    {
        Serial.println("音频模块初始化完成！网络连接成功！");
        getTimeFromServer();
        url = getUrl(websockets_server, "spark-api.xf-yun.com", websockets_server.substring(25), Date);
        url1 = getUrl(websockets_server1, "iat-api.xfyun.cn", "/v2/iat", Date);
        tft.fillScreen(TFT_WHITE);
        u8g2.setCursor(0, 11);
        u8g2.print("网络连接成功！");
        displayWrappedText("请进行语音唤醒或按boot键开始对话！", 0, u8g2.getCursorY() + 12, width);
        awake_flag = 0;
    }
    else
    {
        openWeb();
    }
    urlTime = millis();
    delay(1000);
}

void loop()
{
    webSocketClient.poll();
    webSocketClient1.poll();

    if (startPlay)  voicePlay(); 
    audio2.loop();
    if (audio2.isplaying == 1)  digitalWrite(led, HIGH);   
    else    digitalWrite(led, LOW);    
    

    if (audio2.isplaying == 0 && awake_flag == 0 && await_flag == 1)
    {
        awake_flag = 1;
        StartConversation();
    }

    if (audio2.isplaying == 1 && image_show == 1)
    {
        imageshow();
    }
}


void displayWrappedText(const string &text1, int x, int y, int maxWidth)
{
    int cursorX = x;
    int cursorY = y;
    int lineHeight = u8g2.getFontAscent() - u8g2.getFontDescent() + 2; 
    int start = 0;                                                     
    int num = text1.size();
    int i = 0;

    while (start < num)
    {
        u8g2.setCursor(cursorX, cursorY);
        int wid = 0;
        int numBytes = 0;

        while (i < num)
        {
            int size = 1;
            if (text1[i] & 0x80)
            { 
                char temp = text1[i];
                temp <<= 1;
                do
                {
                    temp <<= 1;
                    ++size;
                } while (temp & 0x80);
            }
            string subWord;
            subWord = text1.substr(i, size); 

            int charBytes = subWord.size(); 

            int charWidth = charBytes == 3 ? 12 : 6; 
            if (wid + charWidth > maxWidth - cursorX)
            {
                break;
            }
            numBytes += charBytes;
            wid += charWidth;

            i += size;
        }

        if (cursorY <= height - 10)
        {
            u8g2.print(text1.substr(start, numBytes).c_str());
            cursorY += lineHeight;
            cursorX = 0;
            start += numBytes;
        }
        else
        {
            text_temp = text1.substr(start).c_str();
            break;
        }
    }
}


DynamicJsonDocument gen_params(const char *appid, const char *domain, const char *role_set)
{
    
    DynamicJsonDocument data(1500);

    
    JsonObject header = data.createNestedObject("header");
    header["app_id"] = appid;
    header["uid"] = "1234";

    
    JsonObject parameter = data.createNestedObject("parameter");

    
    JsonObject chat = parameter.createNestedObject("chat");
    chat["domain"] = domain;
    chat["temperature"] = 0.6;
    chat["max_tokens"] = 1024;

    
    JsonObject payload = data.createNestedObject("payload");

    
    JsonObject message = payload.createNestedObject("message");

    
    JsonArray textArray = message.createNestedArray("text");

    JsonObject systemMessage = textArray.createNestedObject();
    systemMessage["role"] = "system";
    systemMessage["content"] = role_set;

    
    /*for (const auto &item : text)
    {
        textArray.add(item);
    }*/
    
    for (const auto& jsonStr : text) {
        DynamicJsonDocument tempDoc(512);
        DeserializationError error = deserializeJson(tempDoc, jsonStr);
        if (!error) {
            textArray.add(tempDoc.as<JsonVariant>());
        } else {
            Serial.print("反序列化失败: ");
            Serial.println(error.c_str());
        }
    }
    
    return data;
}

DynamicJsonDocument gen_params_http(const char *model, const char *role_set)
{
    
    DynamicJsonDocument data(1500);

    data["model"] = model;
    data["max_tokens"] = 1024;
    data["temperature"] = 0.7;
    data["presence_penalty"] = 1.5;
    data["stream"] = true;

    
    JsonArray textArray = data.createNestedArray("messages");

    JsonObject systemMessage = textArray.createNestedObject();
    systemMessage["role"] = "system";
    systemMessage["content"] = role_set;

    
    /*for (const auto &item : text)
    {
        textArray.add(item);
    }*/
    
    for (const auto& jsonStr : text) {
        DynamicJsonDocument tempDoc(512);
        DeserializationError error = deserializeJson(tempDoc, jsonStr);
        if (!error) {
            textArray.add(tempDoc.as<JsonVariant>());
        } else {
            Serial.print("反序列化失败: ");
            Serial.println(error.c_str());
        }
    }
    
    return data;
}

DynamicJsonDocument gen_params_dify()
{
    
    DynamicJsonDocument data(500);

    JsonObject inputsObj = data.createNestedObject("inputs"); 
    data["query"] = askquestion.c_str();
    data["response_mode"] = "streaming";
    data["conversation_id"] = conversation_id.c_str();
    data["user"] = "abc-123456";

    
    return data;
}

DynamicJsonDocument gen_params_tongyi_app()
{
    
    DynamicJsonDocument data(500);

    JsonObject inputObj = data.createNestedObject("input"); 
    inputObj["prompt"] = askquestion.c_str();
    inputObj["session_id"] = session_id.c_str();
    JsonObject parametersObj = data.createNestedObject("parameters"); 
    parametersObj["incremental_output"] = true;
    JsonObject debugObj = data.createNestedObject("debug"); 

    
    return data;
}

void processResponse(int status)
{
    
    if (Answer.length() >= 180 && (audio2.isplaying == 0) && flag == 0)
    {
        if (Answer.length() >= 300)
        {
            
            int firstPeriodIndex = Answer.indexOf("。");
            if (firstPeriodIndex == -1)
            {
                
                int firstPeriodIndex = Answer.indexOf("；");
            }
            if (firstPeriodIndex == -1)
            {
                
                int firstPeriodIndex = Answer.indexOf("？");
            }
            if (firstPeriodIndex == -1)
            {
                
                int firstPeriodIndex = Answer.indexOf("！");
            }
            
            if (firstPeriodIndex != -1)
            {
                
                String subAnswer1 = Answer.substring(0, firstPeriodIndex + 3);
                Serial.print("subAnswer1:");
                Serial.println(subAnswer1);

                
                audio2.connecttospeech(subAnswer1.c_str(), "zh");

                
                getText("assistant", subAnswer1);
                tft.setCursor(54, 152);
                tft.print(loopcount);
                flag = 1;

                
                Answer = Answer.substring(firstPeriodIndex + 3);
                subAnswer1.clear();
                
                startPlay = true;
            }
            else
            {
                Serial.println("问题里面句号、分号、问号、感叹号断句都没有！");
            }
        }
        else
        {
            
            int lastCommaIndex = Answer.lastIndexOf("，");
            if (lastCommaIndex != -1)
            {
                String subAnswer1 = Answer.substring(0, lastCommaIndex + 3);
                Serial.print("subAnswer1:");
                Serial.println(subAnswer1);
                audio2.connecttospeech(subAnswer1.c_str(), "zh");
                getText("assistant", subAnswer1);
                tft.setCursor(54, 152);
                tft.print(loopcount);
                flag = 1;
                Answer = Answer.substring(lastCommaIndex + 3);
                subAnswer1.clear();
                startPlay = true;
            }
            else
            {  
                String subAnswer1 = Answer.substring(0, Answer.length());
                Serial.print("subAnswer1:");
                Serial.println(subAnswer1);
                audio2.connecttospeech(subAnswer1.c_str(), "zh");
                getText("assistant", subAnswer1);
                tft.setCursor(54, 152);
                tft.print(loopcount);
                flag = 1;
                Answer = Answer.substring(Answer.length());
                subAnswer1.clear();
                startPlay = true;
            }
        }
        conflag = 1;
    }
    
    while (Answer.length() >= 180)
    {
        if (Answer.length() >= 300)
        {
            
            int firstPeriodIndex = Answer.indexOf("。");
            if (firstPeriodIndex == -1)
            {
                
                int firstPeriodIndex = Answer.indexOf("；");
            }
            if (firstPeriodIndex == -1)
            {
                
                int firstPeriodIndex = Answer.indexOf("？");
            }
            if (firstPeriodIndex == -1)
            {
                
                int firstPeriodIndex = Answer.indexOf("！");
            }
            
            if (firstPeriodIndex != -1)
            {
                subAnswers.push_back(Answer.substring(0, firstPeriodIndex + 3));
                Serial.print("subAnswer");
                Serial.print(subAnswers.size() + 1);
                Serial.print("：");
                Serial.println(subAnswers[subAnswers.size() - 1]);

                Answer = Answer.substring(firstPeriodIndex + 3);
            }
            else
            {
                Serial.println("问题里面句号、分号、问号、感叹号断句都没有！");
            }
        }
        else
        {
            int lastCommaIndex = Answer.lastIndexOf("，");
            if (lastCommaIndex != -1)
            {
                subAnswers.push_back(Answer.substring(0, lastCommaIndex + 3));
                Serial.print("subAnswer");
                Serial.print(subAnswers.size() + 1);
                Serial.print("：");
                Serial.println(subAnswers[subAnswers.size() - 1]);

                Answer = Answer.substring(lastCommaIndex + 3);
            }
            else
            {
                subAnswers.push_back(Answer.substring(0, Answer.length()));
                Serial.print("subAnswer");
                Serial.print(subAnswers.size() + 1);
                Serial.print("：");
                Serial.println(subAnswers[subAnswers.size() - 1]);

                Answer = Answer.substring(Answer.length());
            }
        }
    }

    
    if (status == 2 && flag == 0)
    {
        
        audio2.connecttospeech(Answer.c_str(), "zh");
        
        getText("assistant", Answer);
        tft.setCursor(54, 152);
        tft.print(loopcount);
        Answer = "";
        conflag = 1;
        startPlay = true;
    }
}


void onMessageCallback(WebsocketsMessage message)
{
    
    StaticJsonDocument<1024> jsonDocument;

    
    DeserializationError error = deserializeJson(jsonDocument, message.data());

    
    if (!error)
    {
        
        int code = jsonDocument["header"]["code"];

        
        if (code != 0)
        {
            
            Serial.print("sth is wrong: ");
            Serial.println(code);
            Serial.println(message.data());

            
            webSocketClient.close();
        }
        else
        {
            
            JsonObject choices = jsonDocument["payload"]["choices"];

            
            int status = choices["status"];

            
            const char *content = choices["text"][0]["content"];
            const char *removeSet = "\n*$"; 
            
            int length = strlen(content) + 1;
            char *cleanedContent = new char[length];
            removeChars(content, cleanedContent, removeSet);
            Serial.println(cleanedContent);

            
            Answer += cleanedContent;
            content = "";
            
            delete[] cleanedContent;

            processResponse(status);
        }
    }
}


void onEventsCallback(WebsocketsEvent event, String data)
{
    
    if (event == WebsocketsEvent::ConnectionOpened)
    {
        
        Serial.println("Send message to server0!");

        
        DynamicJsonDocument jsonData = gen_params(appId1.c_str(), domain1.c_str(), roleSet1.c_str());

        
        String jsonString;
        serializeJson(jsonData, jsonString);

        
        Serial.println(jsonString);

        
        webSocketClient.send(jsonString);
    }
    
    else if (event == WebsocketsEvent::ConnectionClosed)
    {
        
        Serial.println("Connnection0 Closed");
    }
    
    else if (event == WebsocketsEvent::GotPing)
    {
        
        Serial.println("Got a Ping!");
    }
    
    else if (event == WebsocketsEvent::GotPong)
    {
        
        Serial.println("Got a Pong!");
    }
}


String extractNumber(const String &str) {
  String result;
  for (size_t i = 0; i < str.length(); i++) {
    if (isDigit(str[i])) {
      result += str[i];
    }
  }
  return result;
}


void VolumeSet()
{
    String numberStr = extractNumber(askquestion);
    
    if ((askquestion.indexOf("显示") > -1 && askquestion.indexOf("音") > -1) || (askquestion.indexOf("音") > -1 && askquestion.indexOf("多") > -1))
    {
        Serial.print("当前音量为: ");
        Serial.println(volume);
        
        tft.fillRect(66, 152, 62, 7, TFT_WHITE);
        tft.setCursor(66, 152);
        tft.print("volume:");
        tft.print(volume);
    }
    else if (numberStr.length() > 0)
    {
        volume = numberStr.toInt();
        audio2.setVolume(volume);
        Serial.print("音量已调到: ");
        Serial.println(volume);
        
        tft.fillRect(66, 152, 62, 7, TFT_WHITE);
        tft.setCursor(66, 152);
        tft.print("volume:");
        tft.print(volume);
    }
    else if (askquestion.indexOf("最") > -1 && (askquestion.indexOf("高") > -1 || askquestion.indexOf("大") > -1))
    {
        volume = 100;
        audio2.setVolume(volume);
        Serial.print("音量已调到: ");
        Serial.println(volume);
        
        tft.fillRect(66, 152, 62, 7, TFT_WHITE);
        tft.setCursor(66, 152);
        tft.print("volume:");
        tft.print(volume);
    }
    else if (askquestion.indexOf("高") > -1 || askquestion.indexOf("大") > -1)
    {
        volume += 10;
        if (volume > 100)
        {
            volume = 100;
        }
        audio2.setVolume(volume);
        Serial.print("音量已调到: ");
        Serial.println(volume);
        
        tft.fillRect(66, 152, 62, 7, TFT_WHITE);
        tft.setCursor(66, 152);
        tft.print("volume:");
        tft.print(volume);
    }
    else if (askquestion.indexOf("最") > -1 && (askquestion.indexOf("低") > -1 || askquestion.indexOf("小") > -1))
    {
        volume = 0;
        audio2.setVolume(volume);
        Serial.print("音量已调到: ");
        Serial.println(volume);
        
        tft.fillRect(66, 152, 62, 7, TFT_WHITE);
        tft.setCursor(66, 152);
        tft.print("volume:");
        tft.print(volume);
    }
    else if (askquestion.indexOf("低") > -1 || askquestion.indexOf("小") > -1)
    {
        volume -= 10;
        if (volume < 0)
        {
            volume = 0;
        }
        audio2.setVolume(volume);
        Serial.print("音量已调到: ");
        Serial.println(volume);
        
        tft.fillRect(66, 152, 62, 7, TFT_WHITE);
        tft.setCursor(66, 152);
        tft.print("volume:");
        tft.print(volume);
    }
    conflag = 1;
}


void response()
{
    tft.fillScreen(TFT_WHITE);
    tft.setCursor(0, 0);
    tft.print("assistant: ");
    audio2.connecttospeech(Answer.c_str(), "zh");
    displayWrappedText(Answer.c_str(), tft.getCursorX(), tft.getCursorY() + 11, width);
    Answer = "";
}


void onMessageCallback1(WebsocketsMessage message)
{
    
    DynamicJsonDocument jsonDocument(4096);

    
    DeserializationError error = deserializeJson(jsonDocument, message.data());

    if (error)
    {
        
        Serial.println("error:");
        Serial.println(error.c_str());
        Serial.println(message.data());
        return;
    }
    
    if (jsonDocument["code"] != 0)
    {
        
        Serial.println(message.data());
        
        webSocketClient1.close();
    }
    else
    {
        
        Serial.println("xunfeiyun stt return message:");
        Serial.println(message.data());

        
        JsonArray ws = jsonDocument["data"]["result"]["ws"].as<JsonArray>();

        if (jsonDocument["data"]["status"] != 2)    
        {
            askquestion = "";
        }

        for (JsonVariant i : ws)
        {
            for (JsonVariant w : i["cw"].as<JsonArray>())
            {
                askquestion += w["w"].as<String>();
            }
        }

        
        Serial.println(askquestion);

        
        if (jsonDocument["data"]["status"] == 2)
        {
            
            Serial.println("status == 2");
            webSocketClient1.close();

            
            if ((askquestion.indexOf("声音") == -1 && askquestion.indexOf("音量") == -1) && !((askquestion.indexOf("开") > -1 || askquestion.indexOf("关") > -1) && askquestion.indexOf("灯") > -1) && !(askquestion.indexOf("暂停") > -1 || askquestion.indexOf("恢复") > -1))
            {
                webSocketClient.close();    
                audio2.isplaying = 0;
                startPlay = false;
                Answer = "";
                flag = 0;
                subindex = 0;
                subAnswers.clear();
                text_temp = "";
            }

            if (askquestion.indexOf("九哥"))
            {
                askquestion.replace("九哥", "九歌");
            }

            
            if (await_flag == 1)
            {
                
                if( (askquestion.indexOf("坤坤") > -1 || askquestion.indexOf("小白") > -1 || askquestion.indexOf("集思") > -1 || askquestion.indexOf("九歌") > -1))
                {
                    await_flag = 0;     
                    start_con = 1;      
                    Answer = "喵~我在的，主人。";
                    response();     
                    conflag = 1;
                    return;
                }
                else
                {
                    
                    awake_flag = 0;
                    return;
                }
            }

            
            if (askquestion == "")
            {
                Answer = "喵~主人，我没有听清，请再说一遍吧";
                response();     
                conflag = 1;
            }
            else if (askquestion.indexOf("退下") > -1 || askquestion.indexOf("再见") > -1 || askquestion.indexOf("拜拜") > -1)
            {
                start_con = 0;      
                musicplay = 0;
                Answer = "喵~主人，我先退下了，有事再叫我。";
                response();     
                await_flag = 1;     
                awake_flag = 0;     
            }
            else if (askquestion.indexOf("转个圈") > -1 || askquestion.indexOf("翻个身") > -1 )
            {
                start_con = 0;      
                musicplay = 0;
                Answer = "连接舵机即可转圈圈啦~嘿嘿。";
                response();     
            }
            else if (askquestion.indexOf("断开") > -1 && (askquestion.indexOf("网络") > -1 || askquestion.indexOf("连接") > -1))
            {
                
                WiFi.disconnect(true);
                tft.fillScreen(TFT_WHITE);
                tft.setCursor(0, 0);
                displayWrappedText("网络连接已断开，请重启设备以再次建立连接！", tft.getCursorX(), tft.getCursorY() + 11, width);
                openWeb();
                displayWrappedText("热点ESP32-Setup已开启，密码为12345678，可在浏览器中打开http:
            }
            else if (audio2.isplaying == 1 && askquestion.indexOf("暂停") > -1)
            {
                tft.fillRect(0, 148, 50, 12, TFT_WHITE);     
                if(audio2.isRunning())
                {   
                    Serial.println("已经暂停！");
                    audio2.pauseResume();
                }
                else
                {
                    Serial.println("当前没有音频正在播放！");
                }
            }
            else if (audio2.isplaying == 1 && askquestion.indexOf("恢复") > -1)
            {
                tft.fillRect(0, 148, 50, 12, TFT_WHITE);     
                if(!audio2.isRunning())
                {   
                    Serial.println("已经恢复！");
                    audio2.pauseResume();
                }
                else
                {
                    Serial.println("当前没有音频正在暂停！");
                }
            }
            else if (askquestion.indexOf("声音") > -1 || askquestion.indexOf("音量") > -1)
            {
                tft.fillRect(0, 148, 50, 12, TFT_WHITE);     
                VolumeSet();    

                
            }
            else if (askquestion.indexOf("开") > -1 && askquestion.indexOf("灯") > -1)
            {
                tft.fillRect(0, 148, 50, 12, TFT_WHITE);     
                digitalWrite(light, HIGH);
                conflag = 1;
            }
            else if (askquestion.indexOf("关") > -1 && askquestion.indexOf("灯") > -1)
            {
                tft.fillRect(0, 148, 50, 12, TFT_WHITE);     
                digitalWrite(light, LOW);
                conflag = 1;
            }
            else if (askquestion.indexOf("换") > -1 && askquestion.indexOf("模型") > -1)
            {
                String numberStr = extractNumber(askquestion);
                if (numberStr.length() > 0)
                {
                    if (numberStr.toInt() > llm_name->length())
                    {
                        Answer = "喵~当前只有" + String(llm_name->length()) + "个大模型，没有这个大模型哦";
                    }
                    else
                    {
                        llm = numberStr.toInt() - 1;
                        Answer = "喵~已为你切换为第"+ numberStr + "个模型（" + llm_name[llm] + "）";
                    }
                }
                if (askquestion.indexOf("字节") > -1 || askquestion.indexOf("豆包") > -1)
                {
                    llm = 0;
                    Answer = "喵~已为你切换为豆包大模型";
                }
                if (askquestion.indexOf("讯飞") > -1 || askquestion.indexOf("星火") > -1)
                {
                    llm = 1;
                    Answer = "喵~已为你切换为星火大模型";
                }
                if (askquestion.indexOf("阿里") > -1 || askquestion.indexOf("通义") > -1 || askquestion.indexOf("千问") > -1)
                {
                    if (askquestion.indexOf("智能体") > -1 || askquestion.indexOf("应用") > -1)
                    {
                        llm = 3;
                        Answer = "喵~已为你切换为通义千问智能体应用";
                    }
                    else
                    {
                        llm = 2;
                        Answer = "喵~已为你切换为通义千问大模型";
                    }
                } 
                if (askquestion.indexOf("Chat") > -1 || askquestion.indexOf("Gpt") > -1 || askquestion.indexOf("chat") > -1 || askquestion.indexOf("gpt") > -1)
                {
                    llm = 4;
                    Answer = "喵~已为你切换为Chatgpt大模型";
                }
                if (askquestion.indexOf("Dify") > -1 || askquestion.indexOf("dify") > -1)
                {
                    llm = 5;
                    Answer = "喵~已为你切换为Dify";
                }   
                response();     
                conflag = 1;
            }
            else if (conStatus == 1)
            {
                tft.fillScreen(TFT_WHITE);
                tft.setCursor(0, 0);
                tft.print("user: ");
                displayWrappedText(askquestion.c_str(), tft.getCursorX(), tft.getCursorY() + 11, width);
                cursorY = u8g2.getCursorY() + 1;
                tft.setCursor(0, u8g2.getCursorY() + 2);

                String musicName = "";
                String musicID = "";
                preferences.begin("music_store", true);
                int numMusic = preferences.getInt("numMusic", 0);
                if (musicplay == 1)
                    musicplay = 1;

                if (askquestion.indexOf("不想") > -1 || askquestion.indexOf("暂停") > -1)
                {
                    musicplay = 0;
                    tft.print("assistant: ");
                    Answer = "好的，那主人还有其它吩咐吗？喵~";
                    audio2.connecttospeech(Answer.c_str(), "zh");
                    displayWrappedText(Answer.c_str(), tft.getCursorX(), tft.getCursorY() + 11, width);
                    Answer = "";
                    conStatus = 0;
                    conflag = 1;
                }
                else if (askquestion.indexOf("上一") > -1)
                {
                    musicnum = musicnum - 1 >= 0 ? musicnum - 1 : numMusic - 1;
                    musicName = preferences.getString(("musicName" + String(musicnum)).c_str(), "");
                    musicID = preferences.getString(("musicId" + String(musicnum)).c_str(), "");
                    Serial.println("音乐名称: " + musicName);
                    Serial.println("音乐ID: " + musicID);

                    String audioStreamURL = "https:
                    Serial.println(audioStreamURL.c_str());
                    audio2.connecttohost(audioStreamURL.c_str());
                    
                    if (musicplay == 0)
                        askquestion = "正在播放音乐：" + musicName;
                    else
                        askquestion = "正在顺序播放所有音乐，当前正在播放：" + musicName;
                    Serial.println(askquestion);
                    displayWrappedText(askquestion.c_str(), tft.getCursorX(), tft.getCursorY() + 11, width);
                    startPlay = true;   
                    if (musicplay == 0)
                    {
                        flag = 1;
                        Answer = "音乐播放完了，主人还想听什么音乐吗？喵~";
                    }
                    conflag = 1;
                }
                else if (askquestion.indexOf("下一") > -1)
                {
                    musicnum = musicnum + 1 < numMusic ? musicnum + 1 : 0;
                    musicName = preferences.getString(("musicName" + String(musicnum)).c_str(), "");
                    musicID = preferences.getString(("musicId" + String(musicnum)).c_str(), "");
                    Serial.println("音乐名称: " + musicName);
                    Serial.println("音乐ID: " + musicID);

                    String audioStreamURL = "https:
                    Serial.println(audioStreamURL.c_str());
                    audio2.connecttohost(audioStreamURL.c_str());
                    
                    if (musicplay == 0)
                        askquestion = "正在播放音乐：" + musicName;
                    else
                        askquestion = "正在顺序播放所有音乐，当前正在播放：" + musicName;
                    Serial.println(askquestion);
                    displayWrappedText(askquestion.c_str(), tft.getCursorX(), tft.getCursorY() + 11, width);
                    startPlay = true;   
                    if (musicplay == 0)
                    {
                        flag = 1;
                        Answer = "音乐播放完了，主人还想听什么音乐吗？喵~";
                    }
                    conflag = 1;
                }
                else if ((askquestion.indexOf("再听") > -1 || askquestion.indexOf("再放") > -1 || askquestion.indexOf("再来") > -1) && askquestion.indexOf("一") > -1)
                {
                    musicName = preferences.getString(("musicName" + String(musicnum)).c_str(), "");
                    musicID = preferences.getString(("musicId" + String(musicnum)).c_str(), "");
                    Serial.println("音乐名称: " + musicName);
                    Serial.println("音乐ID: " + musicID);

                    String audioStreamURL = "https:
                    Serial.println(audioStreamURL.c_str());
                    audio2.connecttohost(audioStreamURL.c_str());
                    
                    if (musicplay == 0)
                        askquestion = "正在播放音乐：" + musicName;
                    else
                        askquestion = "正在顺序播放所有音乐，当前正在播放：" + musicName;
                    Serial.println(askquestion);
                    displayWrappedText(askquestion.c_str(), tft.getCursorX(), tft.getCursorY() + 11, width);
                    startPlay = true;   
                    if (musicplay == 0)
                    {
                        flag = 1;
                        Answer = "音乐播放完了，主人还想听什么音乐吗？喵~";
                    }
                    conflag = 1;
                }
                else if (askquestion.indexOf("听") > -1 || askquestion.indexOf("来") > -1 || askquestion.indexOf("放") > -1 || askquestion.indexOf("换") > -1)
                {
                    if (askquestion.indexOf("随便") > -1)
                    {
                        
                        srand(time(NULL));
                        musicnum = rand() % numMusic;
                        musicName = preferences.getString(("musicName" + String(musicnum)).c_str(), "");
                        musicID = preferences.getString(("musicId" + String(musicnum)).c_str(), "");
                        Serial.println("音乐名称: " + musicName);
                        Serial.println("音乐ID: " + musicID);
                    }
                    else if (askquestion.indexOf("连续") > -1 || askquestion.indexOf("顺序") > -1 || askquestion.indexOf("所有") > -1)
                    {
                        musicplay = 1;
                        if (askquestion.indexOf("继续") == -1)
                            musicnum = 0;
                        musicName = preferences.getString(("musicName" + String(musicnum)).c_str(), "");
                        musicID = preferences.getString(("musicId" + String(musicnum)).c_str(), "");
                    }
                    else if (askquestion.indexOf("最喜欢的") > -1 || askquestion.indexOf("最爱的") > -1)
                    {
                        musicName = "Avid";
                        musicID = "1862822901";
                        Serial.println("音乐名称: " + musicName);
                        Serial.println("音乐ID: " + musicID);
                        for (int i = 0; i < numMusic; ++i)
                        {
                            if (preferences.getString(("musicId" + String(i)).c_str(), "") == musicID)  musicnum = i;
                        }
                    }
                    else    
                    {
                        for (int i = 0; i < numMusic; ++i)
                        {
                            musicName = preferences.getString(("musicName" + String(i)).c_str(), "");
                            musicID = preferences.getString(("musicId" + String(i)).c_str(), "");
                            Serial.println("音乐名称: " + musicName);
                            Serial.println("音乐ID: " + musicID);
                            if (askquestion.indexOf(musicName.c_str()) > -1)
                            {
                                Serial.println("找到了！");
                                musicnum = i;
                                break;
                            }
                            else
                            {
                                musicID = "";
                            }
                        }
                    }

                    if (musicID == "") 
                    {
                        Serial.println("未找到对应的音乐！");
                        tft.print("assistant: ");
                        Answer = "主人，曲库里还没有这首歌哦，换一首吧，喵~";
                        audio2.connecttospeech(Answer.c_str(), "zh");
                        displayWrappedText(Answer.c_str(), tft.getCursorX(), tft.getCursorY() + 11, width);
                        Answer = "";
                        conflag = 1;
                    } 
                    else 
                    {
                        String audioStreamURL = "https:
                        Serial.println(audioStreamURL.c_str());
                        audio2.connecttohost(audioStreamURL.c_str());
                        
                        if (musicplay == 0)
                            askquestion = "正在播放音乐：" + musicName;
                        else
                            askquestion = "正在顺序播放所有音乐，当前正在播放：" + musicName;
                        Serial.println(askquestion);
                        displayWrappedText(askquestion.c_str(), tft.getCursorX(), tft.getCursorY() + 11, width);
                        startPlay = true;   
                        if (musicplay == 0)
                        {
                            flag = 1;
                            Answer = "音乐播放完了，主人还想听什么音乐吗？喵~";
                        }
                        conflag = 1;
                    }
                }
                else    
                {
                    musicplay = 0;
                    conStatus = 0;
                    tft.fillScreen(TFT_WHITE);
                    tft.setCursor(0, 0);
                    getText("user", askquestion);
                    if (askquestion.indexOf("天气") > -1 || askquestion.indexOf("几点了") > -1 || askquestion.indexOf("日期") > -1)
                        ConnServer();
                    else
                    {
                        switch (llm)
                        {
                        case 0:
                            doubao();       
                            break;
                        case 1:
                            ConnServer();   
                            break;
                        case 2:
                            tongyi();       
                            break;
                        case 3:
                            tongyi_app();   
                            break;
                        case 4:
                            chatgpt();      
                            break;
                        case 5:
                            dify();         
                            break;
                        default:
                            ConnServer();   
                            break;
                        }
                    }
                }
                preferences.end();
            }
            else if (((askquestion.indexOf("听") > -1 || askquestion.indexOf("放") > -1) && (askquestion.indexOf("歌") > -1 || askquestion.indexOf("音乐") > -1) && askquestion.indexOf("九歌") == -1) || mainStatus == 1)
            {
                tft.fillScreen(TFT_WHITE);
                tft.setCursor(0, 0);
                tft.print("user: ");
                displayWrappedText(askquestion.c_str(), tft.getCursorX(), tft.getCursorY() + 11, width);
                cursorY = u8g2.getCursorY() + 1;
                tft.setCursor(0, u8g2.getCursorY() + 2);

                String musicName = "";
                String musicID = "";
                preferences.begin("music_store", true);
                int numMusic = preferences.getInt("numMusic", 0);

                if (askquestion.indexOf("不想") > -1)
                {
                    mainStatus = 0;
                    tft.print("assistant: ");
                    Answer = "好的，那主人还有其它吩咐吗？喵~";
                    audio2.connecttospeech(Answer.c_str(), "zh");
                    displayWrappedText(Answer.c_str(), tft.getCursorX(), tft.getCursorY() + 11, width);
                    Answer = "";
                    conflag = 1;
                    return;
                }

                if (askquestion.indexOf("随便") > -1)
                {
                    
                    srand(time(NULL));
                    musicnum = rand() % numMusic;
                    musicName = preferences.getString(("musicName" + String(musicnum)).c_str(), "");
                    musicID = preferences.getString(("musicId" + String(musicnum)).c_str(), "");
                    Serial.println("音乐名称: " + musicName);
                    Serial.println("音乐ID: " + musicID);
                }
                else if (askquestion.indexOf("连续") > -1 || askquestion.indexOf("顺序") > -1 || askquestion.indexOf("所有") > -1)
                {
                    musicplay = 1;
                    if (askquestion.indexOf("继续") == -1)
                        musicnum = 0;
                    musicName = preferences.getString(("musicName" + String(musicnum)).c_str(), "");
                    musicID = preferences.getString(("musicId" + String(musicnum)).c_str(), "");
                }
                else if (askquestion.indexOf("最喜欢的") > -1 || askquestion.indexOf("最爱的") > -1)
                {
                    musicName = "Avid";
                    musicID = "1862822901";
                    Serial.println("音乐名称: " + musicName);
                    Serial.println("音乐ID: " + musicID);
                    for (int i = 0; i < numMusic; ++i)
                    {
                        if (preferences.getString(("musicId" + String(i)).c_str(), "") == musicID)  musicnum = i;
                    }
                }
                else    
                {
                    for (int i = 0; i < numMusic; ++i)
                    {
                        musicName = preferences.getString(("musicName" + String(i)).c_str(), "");
                        musicID = preferences.getString(("musicId" + String(i)).c_str(), "");
                        Serial.println("音乐名称: " + musicName);
                        Serial.println("音乐ID: " + musicID);
                        if (askquestion.indexOf(musicName.c_str()) > -1)
                        {
                            Serial.println("找到了！");
                            musicnum = i;
                            break;
                        }
                        else
                        {
                            musicID = "";
                        }
                    }
                }

                if (musicID == "") 
                {
                    mainStatus = 1;
                    Serial.println("未找到对应的音乐！");
                    tft.print("assistant: ");
                    Answer = "好的喵，主人，你想听哪首歌呢，喵~";
                    audio2.connecttospeech(Answer.c_str(), "zh");
                    displayWrappedText(Answer.c_str(), tft.getCursorX(), tft.getCursorY() + 11, width);
                    Answer = "";
                    conflag = 1;
                } 
                else 
                {
                    mainStatus = 0;
                    
                    String audioStreamURL = "https:
                    Serial.println(audioStreamURL.c_str());
                    audio2.connecttohost(audioStreamURL.c_str());

                    if (musicplay == 0)
                        askquestion = "正在播放音乐：" + musicName;
                    else
                        askquestion = "开始顺序播放所有音乐，当前正在播放：" + musicName;
                    Serial.println(askquestion);
                    displayWrappedText(askquestion.c_str(), tft.getCursorX(), tft.getCursorY() + 11, width);
                    startPlay = true;   
                    conStatus = 1;
                    if (musicplay == 0)
                    {
                        flag = 1;
                        Answer = "音乐播放完了，主人还想听什么音乐吗？";
                    }
                    conflag = 1;
                }
                preferences.end();
            }
            else if (askquestion.indexOf("放") > -1 && (askquestion.indexOf("图片") > -1 || askquestion.indexOf("幻灯片") > -1))
            {
                tft.fillScreen(TFT_WHITE);
                tft.setCursor(0, 0);
                getText("user", askquestion);
                Answer = "这就开始放映主人喜欢的图片，喵~";
                audio2.connecttospeech(Answer.c_str(), "zh");
                getText("assistant", Answer);
                Answer = "";
                image_show = 1;
                conflag = 1;
            }
            else    
            {
                tft.fillScreen(TFT_WHITE);
                tft.setCursor(0, 0);
                getText("user", askquestion);
                if (askquestion.indexOf("天气") > -1 || askquestion.indexOf("几点了") > -1 || askquestion.indexOf("日期") > -1)
                    ConnServer();
                else
                {
                    switch (llm)
                    {
                    case 0:
                        doubao();       
                        break;
                    case 1:
                        ConnServer();   
                        break;
                    case 2:
                        tongyi();       
                        break;
                    case 3:
                        tongyi_app();   
                        break;
                    case 4:
                        chatgpt();      
                        break;
                    case 5:
                        dify();         
                        break;  
                    default:
                        ConnServer();   
                        break;
                    }
                }
            }
        }
    }
}


void onEventsCallback1(WebsocketsEvent event, String data)
{
    
    if (event == WebsocketsEvent::ConnectionOpened)
    {
        
        Serial.println("Send message to xunfeiyun stt!");

        
        int silence = 0;
        int firstframe = 1;
        int voicebegin = 0;
        int voice = 0;
        int null_voice = 0;

        
        StaticJsonDocument<2000> doc;

        if (await_flag == 1)
        {
            tft.fillScreen(TFT_WHITE);
            u8g2.setCursor(0, 11);
            u8g2.print("待机中......");
        }
        else if (conflag == 1)
        {
            tft.fillScreen(TFT_WHITE);
            u8g2.setCursor(0, 11);
            u8g2.print("连续对话中，请说话！");
        }
        else
        {
            u8g2.setCursor(0, 159);
            u8g2.print("请说话！");
        }
        conflag = 0;

        Serial.println("开始录音 Start recording......");
        
        while (1)
        {
            
            if (digitalRead(key) == 0 && await_flag == 1)
            {
                start_con = 1;      
                await_flag = 0;
                webSocketClient1.close();
                break;
            }
            
            doc.clear();

            
            JsonObject data = doc.createNestedObject("data");

            
            audio1.Record();

            
            float rms = calculateRMS((uint8_t *)audio1.wavData[0], 1280);
            if (null_voice < 10 && rms > 1000) 
            {
                rms = 8.6;
            }
            printf("RMS: %f\n", rms);

            if(null_voice >= 80)    
            {
                if (start_con == 1)     
                {
                    start_con = 0;      
                    Answer = "主人，我先退下了，有事再找我喵~";
                    response();     
                }
                
                await_flag = 1;
                
                awake_flag = 0;
                
                webSocketClient1.close();
                Serial.println("录音结束 End of recording.");
                return;
            }

            
            if (rms < noise)
            {
                null_voice++;
                if (voicebegin == 1)
                {
                    silence++;
                }
            }
            else
            {
                if (null_voice > 0)
                    null_voice--;
                voice++;
                if (voice >= 5)
                {
                    voicebegin = 1;
                }
                else
                {
                    voicebegin = 0;
                }
                silence = 0;
            }

            
            if (silence == 8)
            {
                data["status"] = 2;
                data["format"] = "audio/L16;rate=8000";
                data["audio"] = base64::encode((byte *)audio1.wavData[0], 1280);
                data["encoding"] = "raw";

                String jsonString;
                serializeJson(doc, jsonString);

                webSocketClient1.send(jsonString);
                delay(40);
                Serial.println("录音结束 End of recording.");
                break;
            }

            
            if (firstframe == 1)
            {
                data["status"] = 0;
                data["format"] = "audio/L16;rate=8000";
                data["audio"] = base64::encode((byte *)audio1.wavData[0], 1280);
                data["encoding"] = "raw";

                JsonObject common = doc.createNestedObject("common");
                common["app_id"] = appId1.c_str();

                JsonObject business = doc.createNestedObject("business");
                business["domain"] = "iat";
                business["language"] = language.c_str();
                business["accent"] = "mandarin";
                
                
                
                business["dwa"] = "wpgs";
                business["vad_eos"] = 2000;

                String jsonString;
                serializeJson(doc, jsonString);

                webSocketClient1.send(jsonString);
                firstframe = 0;
                delay(40);
            }
            else
            {
                
                data["status"] = 1;
                data["format"] = "audio/L16;rate=8000";
                data["audio"] = base64::encode((byte *)audio1.wavData[0], 1280);
                data["encoding"] = "raw";

                String jsonString;
                serializeJson(doc, jsonString);

                webSocketClient1.send(jsonString);
                delay(40);
            }
        }
    }
    
    else if (event == WebsocketsEvent::ConnectionClosed)
    {
        
        Serial.println("Connnection1 Closed");
    }
    
    else if (event == WebsocketsEvent::GotPing)
    {
        
        Serial.println("Got a Ping!");
    }
    
    else if (event == WebsocketsEvent::GotPong)
    {
        
        Serial.println("Got a Pong!");
    }
}





void ConnServer()
{
    
    
    webSocketClient.onMessage(onMessageCallback);       
    webSocketClient.onEvent(onEventsCallback);          

    
    Serial.println("开始连接讯飞星火大模型服务......Begin connect to server0(Xunfei Spark LLM)......");

    
    if (webSocketClient.connect(url.c_str()))
    {
        
        Serial.println("连接LLM成功！Connected to server0(Xunfei Spark LLM)!");
    }
    else
    {
        
        Serial.println("连接LLM失败！Failed to connect to server0(Xunfei Spark LLM)!");
    }
}

void ConnServer1()
{
    

    webSocketClient1.onMessage(onMessageCallback1);
    webSocketClient1.onEvent(onEventsCallback1);

    
    Serial.println("开始连接讯飞STT语音转文字服务......Begin connect to server1(Xunfei STT)......");
    if (webSocketClient1.connect(url1.c_str()))
    {
        Serial.println("连接成功！Connected to server1(Xunfei STT)!");
    }
    else
    {
        Serial.println("连接失败！Failed to connect to server1(Xunfei STT)!");
    }
}

int wifiConnect()
{
    
    WiFi.disconnect(true);

    preferences.begin("wifi_store");
    int numNetworks = preferences.getInt("numNetworks", 0);
    if (numNetworks == 0)
    {
        
        u8g2.setCursor(0, u8g2.getCursorY() + 12);
        u8g2.print("无任何wifi存储信息！");
        displayWrappedText("请连接热点ESP32-Setup密码为12345678，然后在浏览器中打开http:
        preferences.end();
        return 0;
    }

    
    for (int i = 0; i < numNetworks; ++i)
    {
        String ssid = preferences.getString(("ssid" + String(i)).c_str(), "");
        String password = preferences.getString(("password" + String(i)).c_str(), "");

        
        if (ssid.length() > 0 && password.length() > 0)
        {
            Serial.print("Connecting to ");
            Serial.println(ssid);
            Serial.print("password:");
            Serial.println(password);
            
            u8g2.setCursor(0, u8g2.getCursorY()+12);
            u8g2.print(ssid);

            uint8_t count = 0;
            WiFi.begin(ssid.c_str(), password.c_str());
            
            while (WiFi.status() != WL_CONNECTED)
            {
                
                digitalWrite(led, ledstatus);
                ledstatus = !ledstatus;
                count++;

                
                if (count >= 30)
                {
                    Serial.printf("\r\n-- wifi connect fail! --\r\n");
                    
                    u8g2.setCursor(u8g2.getCursorX()+6, u8g2.getCursorY());
                    u8g2.print("Failed!");
                    break;
                }

                
                vTaskDelay(100);
            }

            if (WiFi.status() == WL_CONNECTED)
            {
                
                Serial.printf("\r\n-- wifi connect success! --\r\n");
                Serial.print("IP address: ");
                Serial.println(WiFi.localIP());

                
                Serial.println("Free Heap: " + String(ESP.getFreeHeap()));
                
                u8g2.setCursor(u8g2.getCursorX()+6, u8g2.getCursorY());
                u8g2.print("Connected!");
                preferences.end();
                return 1;
            }
        }
    }
    
    tft.fillScreen(TFT_WHITE);
    
    u8g2.setCursor(0, 11);
    u8g2.print("网络连接失败！请检查");
    u8g2.setCursor(0, u8g2.getCursorY() + 12);
    u8g2.print("网络设备，确认可用后");
    u8g2.setCursor(0, u8g2.getCursorY() + 12);
    u8g2.print("重启设备以建立连接！");
    displayWrappedText("或者连接热点ESP32-Setup密码为12345678，然后在浏览器中打开http:
    preferences.end();
    return 0;
}

void getTimeFromServer()
{
    String timeurl = "https:
    HTTPClient http;                
    http.begin(timeurl);            
    const char *headerKeys[] = {"Date"};        
    http.collectHeaders(headerKeys, sizeof(headerKeys) / sizeof(headerKeys[0]));    
    int httpCode = http.GET();      
    Date = http.header("Date");     
    Serial.println(Date);           
    http.end();                     

    
}

String getUrl(String Spark_url, String host, String path, String Date)
{
    
    String signature_origin = "host: " + host + "\n";
    signature_origin += "date: " + Date + "\n";
    signature_origin += "GET " + path + " HTTP/1.1";
    

    
    unsigned char hmac[32];                                 
    mbedtls_md_context_t ctx;                               
    mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;          
    const size_t messageLength = signature_origin.length(); 
    const size_t keyLength = APISecret.length();            

    
    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 1);
    
    mbedtls_md_hmac_starts(&ctx, (const unsigned char *)APISecret.c_str(), keyLength);
    
    mbedtls_md_hmac_update(&ctx, (const unsigned char *)signature_origin.c_str(), messageLength);
    
    mbedtls_md_hmac_finish(&ctx, hmac);
    
    mbedtls_md_free(&ctx);

    
    String signature_sha_base64 = base64::encode(hmac, sizeof(hmac) / sizeof(hmac[0]));

    
    Date.replace(",", "%2C");
    Date.replace(" ", "+");
    Date.replace(":", "%3A");

    
    String authorization_origin = "api_key=\"" + APIKey + "\", algorithm=\"hmac-sha256\", headers=\"host date request-line\", signature=\"" + signature_sha_base64 + "\"";

    
    String authorization = base64::encode(authorization_origin);

    
    String url = Spark_url + '?' + "authorization=" + authorization + "&date=" + Date + "&host=" + host;

    
    Serial.println(url);

    
    return url;
}


float calculateRMS(uint8_t *buffer, int bufferSize)
{
    float sum = 0;  
    int16_t sample; 

    
    for (int i = 0; i < bufferSize; i += 2)
    {
        
        sample = (buffer[i + 1] << 8) | buffer[i];

        
        sum += sample * sample;
    }

    
    sum /= (bufferSize / 2);

    
    return sqrt(sum);
}


void removeChars(const char *input, char *output, const char *removeSet)
{
    int j = 0;
    for (int i = 0; input[i] != '\0'; ++i)
    {
        bool shouldRemove = false;
        for (int k = 0; removeSet[k] != '\0'; ++k)
        {
            if (input[i] == removeSet[k])
            {
                shouldRemove = true;
                break;
            }
        }
        if (!shouldRemove)
        {
            output[j++] = input[i];
        }
    }
    output[j] = '\0'; 
}


void doubao()
{
    HTTPClient http;
    http.setTimeout(20000);     
    http.begin(apiUrl);
    http.addHeader("Content-Type", "application/json");
    String token_key = String("Bearer ") + doubao_apiKey;
    http.addHeader("Authorization", token_key);

    
    Serial.println("Send message to doubao!");

    
    DynamicJsonDocument jsonData = gen_params_http(model1.c_str(), roleSet.c_str());

    
    String jsonString;
    serializeJson(jsonData, jsonString);

    
    Serial.println(jsonString);
    int httpResponseCode = http.POST(jsonString);

    if (httpResponseCode == 200) {
        Serial.print("httpResponseCode: ");
        Serial.println(httpResponseCode);
        
        WiFiClient* stream = http.getStreamPtr();   

        while (stream->connected()) {   
            String line = stream->readStringUntil('\n');    
            
            
            if (line.startsWith("data:")) {
                
                String data = line.substring(5);
                data.trim();
                
                
                

                int status = 0;
                StaticJsonDocument<400> jsonResponse;
                
                DeserializationError error = deserializeJson(jsonResponse, data);

                
                if (!error)
                {
                    const char *content = jsonResponse["choices"][0]["delta"]["content"];
                    if (jsonResponse["choices"][0]["delta"]["content"] != "")
                    {
                        const char *removeSet = "\n*$"; 
                        
                        int length = strlen(content) + 1;
                        char *cleanedContent = new char[length];
                        removeChars(content, cleanedContent, removeSet);
                        Serial.println(cleanedContent);

                        
                        Answer += cleanedContent;
                        content = "";
                        
                        delete[] cleanedContent;
                    }
                    else
                    {
                        status = 2;
                        Serial.println("status: 2");
                    }

                    processResponse(status);

                    if (status == 2)
                    {
                        stream->stop();
                        break;
                    }
                }
                else {
                    Serial.print("解析错误（Parsing Error）: ");
                    Serial.println(error.c_str());
                }
            }
        }
        /*/ 非流式调用，不推荐，因为没有足够大小的DynamicJsonDocument来存储一次性返回的长文本回复
        String response = http.getString();
        http.end();
        Serial.println(response);

        
        int status = 0;
        DynamicJsonDocument jsonDoc(1024);
        deserializeJson(jsonDoc, response);
        const char *content = jsonDoc["choices"][0]["message"]["content"];
        const char *removeSet = "\n*$"; 
        
        int length = strlen(content) + 1;
        char *cleanedContent = new char[length];
        removeChars(content, cleanedContent, removeSet);
        Serial.println(cleanedContent);

        
        Answer += cleanedContent;
        content = "";
        
        delete[] cleanedContent;
        while (Answer != "")
        {
            if (Answer.length() < 180)
                status = 2;
            processResponse(status);
        }*/
        return;
    } 
    else 
    {
        Serial.printf("Error %i \n", httpResponseCode);
        Serial.println(http.getString());
        http.end();
        return;
    }
}


void tongyi()
{
    HTTPClient http;
    http.setTimeout(20000);     
    http.begin(apiUrl_tongyi);
    String token_key = String("Bearer ") + tongyi_apiKey;
    http.addHeader("Authorization", token_key);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("X-DashScope-SSE", "enable");

    
    Serial.println("Send message to tongyiqianwen!");

    
    DynamicJsonDocument jsonData = gen_params_http(model2.c_str(), roleSet1.c_str());

    
    String jsonString;
    serializeJson(jsonData, jsonString);

    
    Serial.println(jsonString);
    int httpResponseCode = http.POST(jsonString);

    if (httpResponseCode == 200) {
        Serial.print("httpResponseCode: ");
        Serial.println(httpResponseCode);
        
        WiFiClient* stream = http.getStreamPtr();   

        while (stream->connected()) {   
            String line = stream->readStringUntil('\n');    
            
            
            if (line.startsWith("data:")) {
                
                String data = line.substring(5);
                data.trim();
                
                
                

                int status = 0;
                StaticJsonDocument<1024> jsonResponse;
                
                DeserializationError error = deserializeJson(jsonResponse, data);

                
                if (!error)
                {
                    const char *content = jsonResponse["choices"][0]["delta"]["content"];
                    const char *removeSet = "\n*$"; 
                    
                    int length = strlen(content) + 1;
                    char *cleanedContent = new char[length];
                    removeChars(content, cleanedContent, removeSet);
                    Serial.println(cleanedContent);

                    
                    Answer += cleanedContent;
                    content = "";
                    
                    delete[] cleanedContent;

                    if (jsonResponse["choices"][0]["finish_reason"] == "stop")
                    {
                        status = 2;
                        Serial.println("status: 2");
                    }

                    processResponse(status);

                    if (status == 2)
                    {
                        stream->stop();
                        break;
                    }
                }
                else {
                    Serial.print("解析错误（Parsing Error）: ");
                    Serial.println(error.c_str());
                }
            }
        }
        return;
    } 
    else 
    {
        Serial.printf("Error %i \n", httpResponseCode);
        Serial.println(http.getString());
        http.end();
        return;
    }
}


void tongyi_app()
{
    HTTPClient http;
    http.setTimeout(20000);     
    http.begin(apiUrl_tongyi_app);
    String token_key = String("Bearer ") + tongyi_apiKey;
    http.addHeader("Authorization", token_key);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("X-DashScope-SSE", "enable");

    
    Serial.println("Send message to Tongyi_app!");

    
    DynamicJsonDocument jsonData = gen_params_tongyi_app();

    
    String jsonString;
    serializeJson(jsonData, jsonString);

    
    Serial.println(jsonString);
    int httpResponseCode = http.POST(jsonString);

    if (httpResponseCode == 200) {
        Serial.print("httpResponseCode: ");
        Serial.println(httpResponseCode);
        
        WiFiClient* stream = http.getStreamPtr();   

        while (stream->connected()) {   
            String line = stream->readStringUntil('\n');    
            
            
            if (line.startsWith("data:")) {
                
                String data = line.substring(5);
                data.trim();
                
                
                

                int status = 0;
                StaticJsonDocument<1024> jsonResponse;
                
                DeserializationError error = deserializeJson(jsonResponse, data);

                
                if (!error)
                {
                    if (jsonResponse.containsKey("output"))
                    {
                        if (jsonResponse["output"]["finish_reason"] != "stop")
                        {
                            const char *content = jsonResponse["output"]["text"];
                            const char *removeSet = "\n*$"; 
                            
                            int length = strlen(content) + 1;
                            char *cleanedContent = new char[length];
                            removeChars(content, cleanedContent, removeSet);
                            Serial.println(cleanedContent);

                            
                            Answer += cleanedContent;
                            content = "";
                            
                            delete[] cleanedContent;
                        }
                        else
                        {
                            session_id = "";
                            const char *content = jsonResponse["output"]["session_id"];
                            session_id += content;
                            content = "";
                            status = 2;
                            Serial.println("status: 2");
                        }
                    }

                    processResponse(status);

                    if (status == 2)
                    {
                        stream->stop();
                        break;
                    }
                }
                else {
                    Serial.print("解析错误（Parsing Error）: ");
                    Serial.println(error.c_str());
                }
            }
        }
        return;
    } 
    else 
    {
        Serial.printf("Error %i \n", httpResponseCode);
        Serial.println(http.getString());
        http.end();
        return;
    }
}


void chatgpt()
{
    HTTPClient http;
    http.setTimeout(20000);     
    http.begin(apiUrl_chatgpt);
    http.addHeader("Content-Type", "application/json");
    String token_key = String("Bearer ") + openai_apiKey;
    http.addHeader("Authorization", token_key);

    
    Serial.println("Send message to chatgpt!");

    
    DynamicJsonDocument jsonData = gen_params_http(model3.c_str(), roleSet1.c_str());

    
    String jsonString;
    serializeJson(jsonData, jsonString);

    
    Serial.println(jsonString);
    int httpResponseCode = http.POST(jsonString);

    if (httpResponseCode == 200) {
        Serial.print("httpResponseCode: ");
        Serial.println(httpResponseCode);
        
        WiFiClient* stream = http.getStreamPtr();   

        while (stream->connected()) {   
            String line = stream->readStringUntil('\n');    
            
            
            if (line.startsWith("data:")) {
                
                String data = line.substring(5);
                data.trim();
                
                
                

                int status = 0;
                StaticJsonDocument<1024> jsonResponse;
                
                DeserializationError error = deserializeJson(jsonResponse, data);

                
                if (!error)
                {
                    if (jsonResponse["choices"][0]["finish_reason"] == "stop")
                    {
                        status = 2;
                        Serial.println("status: 2");
                    }
                    else
                    {
                        const char *content = jsonResponse["choices"][0]["delta"]["content"];
                        const char *removeSet = "\n*$"; 
                        
                        int length = strlen(content) + 1;
                        char *cleanedContent = new char[length];
                        removeChars(content, cleanedContent, removeSet);
                        Serial.println(cleanedContent);

                        
                        Answer += cleanedContent;
                        content = "";
                        
                        delete[] cleanedContent;
                    }

                    processResponse(status);

                    if (status == 2)
                    {
                        stream->stop();
                        break;
                    }
                }
                else {
                    Serial.print("解析错误（Parsing Error）: ");
                    Serial.println(error.c_str());
                }
            }
        }
        return;
    } 
    else 
    {
        Serial.printf("Error %i \n", httpResponseCode);
        Serial.println(http.getString());
        http.end();
        return;
    }
}


void dify()
{
    HTTPClient http;
    http.setTimeout(20000);     
    http.begin(apiUrl_dify);
    http.addHeader("Authorization", "Bearer " + dify_apiKey);
    http.addHeader("Content-Type", "application/json");

    
    Serial.println("Send message to Dify!");

    
    DynamicJsonDocument jsonData = gen_params_dify();

    
    String jsonString;
    serializeJson(jsonData, jsonString);

    
    Serial.println(jsonString);
    int httpResponseCode = http.POST(jsonString);

    if (httpResponseCode == 200) {
        Serial.print("httpResponseCode: ");
        Serial.println(httpResponseCode);
        
        WiFiClient* stream = http.getStreamPtr();   

        while (stream->connected()) {   
            String line = stream->readStringUntil('\n');    
            
            
            if (line.startsWith("data:")) {
                
                String data = line.substring(5);
                data.trim();
                
                
                

                int status = 0;
                StaticJsonDocument<1024> jsonResponse;
                
                DeserializationError error = deserializeJson(jsonResponse, data);

                
                if (!error)
                {
                    if (jsonResponse.containsKey("answer"))
                    {
                        if (jsonResponse["answer"] != "")
                        {
                            const char *content = jsonResponse["answer"];
                            const char *removeSet = "\n*$"; 
                            
                            int length = strlen(content) + 1;
                            char *cleanedContent = new char[length];
                            removeChars(content, cleanedContent, removeSet);
                            Serial.println(cleanedContent);

                            
                            Answer += cleanedContent;
                            content = "";
                            
                            delete[] cleanedContent;
                        }
                        else
                        {
                            conversation_id = "";
                            const char *content = jsonResponse["conversation_id"];
                            conversation_id += content;
                            content = "";
                            status = 2;
                            Serial.println("status: 2");
                        }
                    }

                    processResponse(status);

                    if (status == 2)
                    {
                        stream->stop();
                        break;
                    }
                }
                else {
                    Serial.print("解析错误（Parsing Error）: ");
                    Serial.println(error.c_str());
                }
            }
        }
        return;
    } 
    else 
    {
        Serial.printf("Error %i \n", httpResponseCode);
        Serial.println(http.getString());
        http.end();
        return;
    }
}
