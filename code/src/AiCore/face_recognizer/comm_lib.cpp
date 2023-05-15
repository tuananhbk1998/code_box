/*
  Copyright (C) 2017 Open Intelligent Machines Co.,Ltd

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#include "comm_lib.hpp"

void nms_boxes(std::vector<face_box>& input, float threshold, int type, std::vector<face_box>&output)
{

	std::sort(input.begin(), input.end(),
		[](const face_box& a, const face_box&b) 
	{
		return a.score > b.score;
	});

	int box_num = input.size();

	std::vector<int> merged(box_num, 0);

	for (int i = 0;i<box_num;i++)
	{
		if (merged[i])
			continue;

		output.push_back(input[i]);

		float h0 = input[i].y1 - input[i].y0 + 1;
		float w0 = input[i].x1 - input[i].x0 + 1;

		float area0 = h0*w0;


		for (int j = i + 1;j<box_num;j++)
		{
			if (merged[j])
				continue;

			float inner_x0 = std::max(input[i].x0, input[j].x0);
			float inner_y0 = std::max(input[i].y0, input[j].y0);

			float inner_x1 = std::min(input[i].x1, input[j].x1);
			float inner_y1 = std::min(input[i].y1, input[j].y1);

			float inner_h = inner_y1 - inner_y0 + 1;
			float inner_w = inner_x1 - inner_x0 + 1;


			if (inner_h <= 0 || inner_w <= 0)
				continue;

			float inner_area = inner_h*inner_w;

			float h1 = input[j].y1 - input[j].y0 + 1;
			float w1 = input[j].x1 - input[j].x0 + 1;

			float area1 = h1*w1;

			float score;

			if (type == NMS_UNION)
			{
				score = inner_area / (area0 + area1 - inner_area);
			}
			else
			{
				score = inner_area / std::min(area0, area1);
			}


			if (score>threshold)
				merged[j] = 1;
		}

	}

}

void regress_boxes(std::vector<face_box>& rects)
{
	for (unsigned int i = 0;i<rects.size();i++)
	{
		face_box& box = rects[i];

		float h = box.y1 - box.y0 + 1;
		float w = box.x1 - box.x0 + 1;

		box.x0 = box.x0 + w*box.regress[0];
		box.y0 = box.y0 + h*box.regress[1];
		box.x1 = box.x1 + w*box.regress[2];
		box.y1 = box.y1 + h*box.regress[3];
	}

}

void square_boxes(std::vector<face_box>& rects)
{

	for (unsigned int i = 0;i<rects.size();i++)
	{
		float h = rects[i].y1 - rects[i].y0 + 1;
		float w = rects[i].x1 - rects[i].x0 + 1;

		float l = std::max(h, w);

		rects[i].x0 = rects[i].x0 + (w - l)*0.5;
		rects[i].y0 = rects[i].y0 + (h - l)*0.5;
		rects[i].x1 = rects[i].x0 + l - 1;
		rects[i].y1 = rects[i].y0 + l - 1;
	}
}

void padding(int img_h, int img_w, std::vector<face_box>& rects)
{
	for (unsigned int i = 0; i<rects.size();i++)
	{
		rects[i].px0 = std::max(rects[i].x0, 1.0f);
		rects[i].py0 = std::max(rects[i].y0, 1.0f);
		rects[i].px1 = std::min(rects[i].x1, (float)img_w);
		rects[i].py1 = std::min(rects[i].y1, (float)img_h);
	}
}


void process_boxes(std::vector<face_box>& input, int img_h, int img_w, std::vector<face_box>& rects)
{

	nms_boxes(input, 0.7 , NMS_UNION, rects);

	regress_boxes(rects);

	square_boxes(rects);

	padding(img_h, img_w, rects);

}


int make_round(float num)
{
	int a = int(num);
	float b = num - a;
	if (int(10 * b) != 5)
	{
		return round(num);
	}
	else
	{
		if (a % 2 == 0)
			return a;
		else
			return a + 1;
	}
}


void generate_bounding_box(const float * confidence_data, int confidence_size,
	const float * reg_data, float scale, float threshold,
	int feature_h, int feature_w, std::vector<face_box>&  output, bool transposed)
{
	///printf("generate_bounding_box: %d, %f, %f, %d, %d\n", confidence_size, scale, threshold, feature_h, feature_w);
	int stride = 2;
	int cellSize = 12;

	int img_h = feature_h;
	int img_w = feature_w;

	int count = confidence_size / 2;
	confidence_data += count;
	int hhh = 0;
	for (int i = 0; i<count; i++) {
		if (*(confidence_data + i) >= threshold)
		{
			hhh++;
			int y = i / img_w;
			int x = i - img_w * y;
			//std::cout<<"x : "<<x<<std::endl;
			//std::cout<<"y : "<<y<<std::endl;

			float top_x = make_round((x*stride + 1) / scale);
			float top_y = make_round((y*stride + 1) / scale);
			float bottom_x = make_round((x*stride + cellSize + 1) / scale);
			float bottom_y = make_round((y*stride + cellSize + 1) / scale);

			face_box box;

			box.x0 = top_x;
			box.y0 = top_y;
			box.x1 = bottom_x;
			box.y1 = bottom_y;

			box.score = *(confidence_data + i);

			int c_offset = y*img_w + x;
			int c_size = img_w*img_h;

			if (transposed)
			{
				box.regress[1] = reg_data[c_offset];
				box.regress[0] = reg_data[c_offset + c_size];
				box.regress[3] = reg_data[c_offset + 2 * c_size];
				box.regress[2] = reg_data[c_offset + 3 * c_size];
			}
			else 
			{
				box.regress[0] = reg_data[c_offset];
				box.regress[1] = reg_data[c_offset + c_size];
				box.regress[2] = reg_data[c_offset + 2 * c_size];
				box.regress[3] = reg_data[c_offset + 3 * c_size];
			}

			output.push_back(box);
			
		}
	}
	printf("--------------Size box: %d\n", hhh);
}


void set_input_buffer(std::vector<cv::Mat>& input_channels,
	float* input_data, const int height, const int width)
{
	for (int i = 0; i < 3; ++i) {
		cv::Mat channel(height, width, CV_32FC1, input_data);
		input_channels.push_back(channel);
		input_data += width * height;
	}
}


void  cal_pyramid_list(int height, int width, int min_size, float factor, std::vector<scale_window>& list)
{
	printf("Size img: w: %d, ,h: %d, min_size: %d, factor: factor: %f\n", width, height, min_size, factor);
	int min_side = std::min(height, width);
	double m = 12.0 / min_size;

	min_side = min_side*m;
	double cur_scale = 1.0;

	double scale;


	while (min_side >= 12)
	{
		scale = m*cur_scale;
		cur_scale = cur_scale *factor;
		min_side *= factor;

		int hs = std::ceil(height*scale);
		int ws = std::ceil(width*scale);

		scale_window win;
		win.h = hs;
		win.w = ws;
		win.scale = scale;
		list.push_back(win);
	}

}

void cal_landmark(std::vector<face_box>& box_list)
{
	for (unsigned int i = 0;i<box_list.size();i++)
	{
		face_box& box = box_list[i];

		float w = box.x1 - box.x0 + 1;
		float h = box.y1 - box.y0 + 1;

		for (int j = 0;j<5;j++)
		{
			box.landmark.x[j] = box.x0 + w*box.landmark.x[j] - 1;
			box.landmark.y[j] = box.y0 + h*box.landmark.y[j] - 1;
		}

	}
}

void set_box_bound(std::vector<face_box>& box_list, int img_h, int img_w)
{
	for (unsigned int i = 0; i<box_list.size();i++)
	{
		face_box& box = box_list[i];

		box.x0 = std::max((int)(box.x0), 1);
		box.y0 = std::max((int)(box.y0), 1);
		box.x1 = std::min((int)(box.x1), img_w - 1);
		box.y1 = std::min((int)(box.y1), img_h - 1);

	}

}

void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
	std::string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (std::string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}