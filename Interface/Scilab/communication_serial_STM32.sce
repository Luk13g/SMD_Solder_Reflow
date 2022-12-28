//Communicating with the STM32 via Serial
clc;
clear;
TCL_EvalStr('foreach chan [file channels] {close $chan}');//Closes all serials opened by Scilab

//Serial opening
Serial_communication = openserial("COM4","9600,n,8,1") 

if Serial_communication == -1 then
  disp("Usb communication error");

  else
  disp("Usb communication OK");
end
tic()
//Serial reading
while Serial_communication ~= -1 
    [q, flags] = serialstatus(Serial_communication)
    if q(1) > 0 then
        Serial_data = readserial(Serial_communication);
        [n,T,t,d] = msscanf(-1,[Serial_data],'%f;%f;%f');
        //disp(t)
        if(n>0) then
            plot([t],[T],'-bo')
            xlabel('t / s')
            ylabel('T / °C')
            legend('T')
            //sleep(500);
        end
    end
end 

closeserial(Serial_communication); 


