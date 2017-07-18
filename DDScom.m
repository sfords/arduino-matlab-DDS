%{
this program is to be used in conjunction with the file wavegen_matlab.ino
uploaded to an arduino connected by the USB serial port.
it serves as a proof of concept for live user control of an arduino program
using the computer. 
type in given commands into the command window to set variables and program parameters
in the arduino and run a frequency sweep using the AD9850
%}

clear;
%arduino('COM3','Mega2560');
srl = serial('COM3','BaudRate',9600);
srl.ReadAsyncMode = 'continuous';

%matlab & control loop housekeeping
mainloop = 1; %main control loop
menuloop = 1; %manu input loop
numloop = 1; %numerical input
outloop = 1; %outputloop
readloop = 1; %serial input data loop
%data housekeeping
num = '';
mode = '';
dataIn = 0;
dataOut = '';

try
    fopen(srl);
    
    handshake = fscanf(srl, '%s');
    if (handshake == 'Z')
        fprintf('Serial port connected');
        fprintf(srl, handshake);
        pause(.3);
    end
    flushinput(srl);
    
    while mainloop == 1
        if menuloop == 1;
            numloop = 1;
            outloop = 1;
            
            prompt = '\ncommands:\nminFreq\nmaxFreq\nfStep\nTmulti\nsweep\nend\n\n';
            command = input(prompt, 's');
        
            if strcmpi(command, 'minFreq')
                %set sweep's minimum frequency 
                mode = 'A';
            elseif strcmp(command, 'maxFreq')
                % set sweep's maximum frequency
                mode = 'B';
            elseif strcmpi(command, 'fStep')
                % set step step size 
                mode = 'C';
            elseif strcmpi(command, 'Tmulti')
                %set time per step, T*82us
                mode = 'D';
            elseif strcmpi (command, 'sweep')
                %start sweep
                mode = 'R';
                numloop = 0;
                readloop = 1;
            elseif strcmpi (command, 'read') 
                % read any leftover data in inputbuffer
                readloop = 1;
                numloop=0;
                outloop = 0;
            elseif strcmpi(command, 'end') 
                %skip all control loops and close program
                mainloop = 0;
                numloop = 0;
                outloop = 0;
                menuloop= 0;
                readloop = 0;
                flushinput(srl);
            else 
                disp('try again');
                numloop = 0;
            end
        end %menuloop
        
        if numloop == 1 
            num = input('enter value: ', 's');
        end %numloop
        
        if outloop == 1
            dataOut = [mode num]; %format and data for Arduino
            disp(['dataOut = ', dataOut]);
            fprintf(srl, dataOut);
            dataOut = '';
            num = '';
            mode = '';
            pause(.3);
        end %outloop
        while srl.BytesAvailable == 0;
        end
        while srl.BytesAvailable > 0 && readloop == 1
            dataIn = 0;
            dataIn = fscanf(srl, '%s');
            disp(dataIn);
                
        end %readloop
    end %end mainloop and program
    
    %end of interface, close port
    fclose(srl);
    clear srl;
    disp('Port closed');
catch me
    %catch error and close port
    fclose(srl);
    clear srl;
    disp(me.message);
end