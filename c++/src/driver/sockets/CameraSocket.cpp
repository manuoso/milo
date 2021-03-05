//---------------------------------------------------------------------------------------------------------------------
//  MILO
//---------------------------------------------------------------------------------------------------------------------
//  Copyright 2020 Manuel Pérez Jiménez (a.k.a. manuoso) manuperezj@gmail.com
//---------------------------------------------------------------------------------------------------------------------
//  Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
//  and associated documentation files (the "Software"), to deal in the Software without restriction, 
//  including without limitation the rights to use, copy, modify, merge, publish, distribute, 
//  sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all copies or substantial 
//  portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
//  BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
//  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES 
//  OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
//  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//---------------------------------------------------------------------------------------------------------------------

#include "milo/driver/sockets/CameraSocket.h"

namespace milo{
    // Notes on Tello video:
    // -- frames are always 960x720.
    // -- frames are split into UDP packets of length 1460.
    // -- normal frames are ~10k, or about 8 UDP packets.
    // -- keyframes are ~35k, or about 25 UDP packets.
    // -- keyframes are always preceded by an 8-byte UDP packet and a 13-byte UDP packet -- markers?
    // -- the h264 parser will consume the 8-byte packet, the 13-byte packet and the entire keyframe without
    //    generating a frame. Presumably the keyframe is stored in the parser and referenced later.

    //---------------------------------------------------------------------------------------------------------------------
    CameraSocket::CameraSocket(int _port)
    {
        if(create(_port)){
            buffer_ = std::vector<unsigned char>(2048);
            seq_buffer_ = std::vector<unsigned char>(65536);
            listen();
        }else{
            std::cout << "[CAMERA_SOCKET] Socket not initialized" << std::endl;
        }
    }

    //---------------------------------------------------------------------------------------------------------------------
    CameraSocket::~CameraSocket(){
        close();
        delete socket_;
    }

    //---------------------------------------------------------------------------------------------------------------------
    cv::Mat CameraSocket::getFrame(){
        std::lock_guard<std::mutex> lock(mtx_);
        return frameDecoded_;
    }

    //---------------------------------------------------------------------------------------------------------------------
    void CameraSocket::process_packet(size_t r)
    {
        std::lock_guard<std::mutex> lock(mtx_);

        // std::cout << "Size camera packet: " << r << std::endl;
        // std::cout << "Buffer num packets: " << seq_buffer_num_packets_ << std::endl;

        receiveTime_ = std::chrono::high_resolution_clock::now();

        if (!receiving_) {
            receiving_ = true;
            seq_buffer_next_ = 0;
            seq_buffer_num_packets_ = 0;
        }

        if (seq_buffer_next_ + r >= seq_buffer_.size()) {
            // std::cout << "Video buffer overflow, dropping sequence" + "\n";
            seq_buffer_next_ = 0;
            seq_buffer_num_packets_ = 0;
            return;
        }

        std::copy(buffer_.begin(), buffer_.begin() + r, seq_buffer_.begin() + seq_buffer_next_);
        seq_buffer_next_ += r;
        seq_buffer_num_packets_++;

        // If the packet is < 1460 bytes then it's the last packet in the sequence
        if(r < 1460){
            decode_frames();

            seq_buffer_next_ = 0;
            seq_buffer_num_packets_ = 0;
        }
    }

    //---------------------------------------------------------------------------------------------------------------------
    void CameraSocket::decode_frames()
    {
        size_t next = 0;

        try {
            while (next < seq_buffer_next_) {
                ssize_t consumed = decoder_.parse(seq_buffer_.data() + next, seq_buffer_next_ - next);

                if (decoder_.is_frame_available()) {
                    const AVFrame &frame = decoder_.decode_frame();

                    // Convert pixels from YUV420P to BGR24
                    int size = converter_.predict_size(frame.width, frame.height);
                    unsigned char bgr24[size];
                    converter_.convert(frame, bgr24);

                    cv::Mat mat{frame.height, frame.width, CV_8UC3, bgr24};
                    cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);

                    // cv::imshow("Tello Camera", mat);
                    // cv::waitKey(1);

                    frameDecoded_ = mat.clone();
                }
                next += consumed;
            }
        }catch (std::runtime_error &e) {
            std::cerr << e.what() << std::endl;
        }
    }

}