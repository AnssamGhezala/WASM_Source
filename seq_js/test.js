let getIndexOfWindow = (radius, grid_number) => {
	return radius * (grid_number / radius) * 4 * 4 + (grid_number % radius) * 8;
};

//calculates the max value of the parsed array
let find_maximum = (a, n) => {
	let c,
		max = 0;

	for (c = 0; c < n; c++) if (a[c] > max) max = a[c];
	return max;
};

let pooling = (index, color_at_pixel, width, image, new_image, thread_id) => {
	// pool [pixel[i], pixel[i+1], pixel[i+width], pixel[i+width+1]] of image to get 1 pixel for new_image
	for (let i = index; i < index + 4; i++) {
		let grid = [ image[i], image[i + 4], image[i + width * 4], image[i + (width + 1) * 4] ];
		new_image[color_at_pixel + (i - index)] = find_maximum(grid, 4);
	}
};

let process = (height, width, num_threads, threadIdx, original_image, new_image, windows_per_thread) => {
	let count = 0;
	// Radius helps us read through the 1d array image, as if it were a 2D array with width/2 windows on the x and y axis
	let radius = width / 2;

	// As long as the current thread has not completed its quota of windows (4 pixel grids to do pooling on), do pooling
	for (let i = 0; i < windows_per_thread; i++) {
		// start index to get where on the image the thread should start doing the pooling
		let index_at_new_image = (threadIdx * windows_per_thread + i) * 4;
		pooling(
			getIndexOfWindow(radius, threadIdx * windows_per_thread + i),
			index_at_new_image,
			width,
			original_image,
			new_image,
			threadIdx
		);
	}

	// If I am at the last thread and I have windows (grids od 4 pixels to be pooled) that have not been pooled...
	if (threadIdx == num_threads - 1) {
		let remaining_windows = (width * height / 4) % num_threads;
		if (remaining_windows > 0) {
			for (let i = 0; i < remaining_windows; i++) {
				// Get the start index of pooling at the remaining window
				let index_at_new_image = ((threadIdx + 1) * windows_per_thread + i) * 4;
				pooling(
					getIndexOfWindow(radius, (threadIdx + 1) * windows_per_thread + i),
					index_at_new_image,
					width,
					original_image,
					new_image,
					threadIdx
				);
			}
		}
	}
};

let testing = () => {
	let array = [];
	for (let i = 0; i < 4 * 1000 * 1000; i++) {
		array[i] = Math.floor(Math.random() * 255);
	}

	let width = 1900;
	let height = 1200;

	// Number of 2x2 grids containing 4 pixels to do pooling on
	let num_windows = width * height / 4;

	// Number of 2x2 grids containing 4 pixels to do pooling on to give per thread to process
	let windows_per_thread = num_windows / 1;

	let new_image = [];
	var t0 = performance.now();

	process(height, width, 1, 0, array, new_image, windows_per_thread);

	var t1 = performance.now();
	console.log('Done in ' + (t1 - t0) + ' milliseconds.');
};

testing();
