const path = require('path');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const MiniCssExtractPlugin = require('mini-css-extract-plugin');

module.exports = {
    mode: 'development',
    devtool: 'eval-source-map',    
    entry: {
        main: './src/js/index.js',  // Assuming this is the main JS for what was previously index.html
        home: './src/js/home.js'    // If you have a separate JS for home.html
    }, // Update the entry point to your index.js in the js folder
    output: {
        filename: '[name].bundle.js',
        path: path.resolve(__dirname, 'dist'),
    },
    module: {
        rules: [
            {
                test: /\.js$/,
                exclude: /node_modules/,
                use: {
                    loader: 'babel-loader',
                    options: {
                        presets: ['@babel/preset-env']
                    }
                },
            },
            {
                test: /\.css$/,
                use: [
                    MiniCssExtractPlugin.loader,
                    'css-loader',
                ],
            },
            // Add other rules as needed
        ],
    },
    plugins: [

        new HtmlWebpackPlugin({
            template: './src/pages/index.html',
            filename: 'index.html',
            chunks: ['main'], // This line is important
        }),        
        new HtmlWebpackPlugin({
            template: './src/pages/home.html',  // This is your previous index.html
            filename: 'home.html',
            chunks: ['home']
        }),
        new MiniCssExtractPlugin({
            filename: 'style.css', // Extracts CSS into a separate file
        }),
    ],

    devServer: {
        historyApiFallback: true,
        static: path.join(__dirname, 'dist'),
        compress: true,
        port: 9000,
        open: true,
        hot: true,
    },
};
